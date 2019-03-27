package ru.hse.spb.client;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.client.commands.AbstractCommand;
import ru.hse.spb.client.commands.Command;
import ru.hse.spb.client.commands.CommandException;
import ru.hse.spb.client.util.BooleanHolder;
import ru.hse.spb.client.util.CommunicationUtil;
import ru.hse.spb.client.util.Pair;
import ru.hse.spb.protocol.Constants;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.deserialization.RequestDeserializer;
import ru.hse.spb.protocol.request.*;
import ru.hse.spb.protocol.response.*;
import ru.hse.spb.protocol.serialization.RequestSerializer;
import ru.hse.spb.protocol.serialization.ResponseSerializer;

import java.io.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.AsynchronousChannelGroup;
import java.nio.channels.AsynchronousSocketChannel;
import java.nio.channels.CompletionHandler;
import java.util.*;
import java.util.concurrent.*;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class Client {
    private static final int DOWNLOADER_THREAD_POOL_SIZE = 4;
    private static final int SERVER_CONNECTION_THREAD_POOL_SIZE = 2;
    private static final int SERVER_THREAD_POOL_SIZE = 10;
    private static final int UPDATE_DELAY = 4; // minutes
    private static final int FILE_QUEUE_SIZE = 20;
    private static final @NotNull String DESTINATION_FOLDER = "downloads/";

    private final @NotNull SeedServer server = new SeedServer();
    private final @NotNull ScheduledExecutorService updaterExecutionService = Executors.newScheduledThreadPool(SERVER_CONNECTION_THREAD_POOL_SIZE);
    private final @NotNull ConcurrentHashMap<Integer, ConcurrentHashMap<Integer, FilePartStatus>> filesMap = new ConcurrentHashMap<>();
    private final @NotNull ConcurrentHashMap<Integer, List<List<InetSocketAddress>>> sourcesMap = new ConcurrentHashMap<>();
    private final @NotNull ConcurrentHashMap<Integer, File> downloadedFiles = new ConcurrentHashMap<>();
    private volatile @NotNull Map<Integer, ListResponse.FileInfo> filesOnServer = new HashMap<>();

    private final @NotNull ExecutorService downloaderThreadPool = Executors.newFixedThreadPool(DOWNLOADER_THREAD_POOL_SIZE);
    private final @NotNull AsynchronousChannelGroup clientsChannelGroup = AsynchronousChannelGroup.withThreadPool(downloaderThreadPool);

    private final @NotNull Map<@NotNull String, @NotNull AbstractCommand> commands = Arrays.stream(Client.class.getDeclaredClasses())
            .filter(clazz -> clazz.isAnnotationPresent(Command.class))
            .filter(AbstractCommand.class::isAssignableFrom)
            .map(clazz -> {
                try {
                    Constructor<?> constructor = clazz.getDeclaredConstructor(Client.class);
                    constructor.setAccessible(true);
                    return new Pair<>(clazz.getAnnotation(Command.class).value(), (AbstractCommand) constructor.newInstance(this));
                } catch (InstantiationException | IllegalAccessException | NoSuchMethodException | InvocationTargetException e) {
                    return null;
                }
            })
            .filter(Objects::nonNull)
            .collect(Collectors.toMap(Pair::getFirst, Pair::getSecond));

    private final @NotNull String serverAddress;
    private volatile boolean working = true;

    enum FilePartStatus {
        SEEDING,
        DOWNLOADING,
        NOT_LOADED
    }

    public Client(@NotNull String serverAddress) throws IOException {
        this.serverAddress = serverAddress;
        new File(DESTINATION_FOLDER).mkdir();
    }

    public void run() {
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(System.in))) {
            final Thread thread = new Thread(server::start);
            thread.setDaemon(true);
            thread.start();
            final UpdateCommand updateCommand = new UpdateCommand();
            updaterExecutionService.scheduleWithFixedDelay(() -> {
                try {
                    updateCommand.processCommand();
                } catch (CommandException e) {
                    e.printStackTrace();
                }
            }, 0, UPDATE_DELAY, TimeUnit.MINUTES);
            while (working) {
                System.out.print("> ");
                final List<String> input = Arrays.asList(reader.readLine().split(" "));
                if (input.isEmpty()) continue;
                final String commandName = input.get(0);
                final List<String> args = input.subList(1, input.size());
                processCommand(commandName, args);
            }
        } catch (Throwable e) {
            // TODO: serialize state
            e.printStackTrace();
        } finally {
            server.stop();
            updaterExecutionService.shutdownNow();
        }
    }

    private void processCommand(@NotNull String commandName, @NotNull List<String> args) {
        AbstractCommand command = commands.get(commandName);
        if (command == null) {
            System.out.println("Incorrect command. Type `help` to see list of all commands");
            return;
        }
        try {
            command.processCommand(args);
        } catch (CommandException e) {
            System.out.println("Error while processing command: " + e.getMessage());
        }
    }

    private @NotNull Socket createServerConnection() throws IOException {
        return new Socket(serverAddress, Constants.SERVER_PORT);
    }

    private class SeedServer {
        private final @NotNull ServerSocket serverSocket = new ServerSocket(0);
        private final @NotNull ExecutorService requestThreadPool = Executors.newFixedThreadPool(SERVER_THREAD_POOL_SIZE);
        private final @NotNull ExecutorService responseThreadPool = Executors.newFixedThreadPool(SERVER_THREAD_POOL_SIZE);
        private final @NotNull ExecutorService fileReader = Executors.newSingleThreadScheduledExecutor();

        SeedServer() throws IOException {
        }

        int getServerPort() {
            return serverSocket.getLocalPort();
        }

        void start() {
            while (working) {
                try {
                    Socket socket = serverSocket.accept();
                    requestThreadPool.submit(new RequestWorker(socket));
                } catch (IOException e) {
                    // TODO
                    e.printStackTrace();
                }
            }
        }

        void stop() {
            try {
                serverSocket.close();
                requestThreadPool.shutdownNow();
            } catch (IOException e) {
                // TODO
                e.printStackTrace();
            }
        }

        private class FileReader implements Runnable {
            private final @NotNull ResponseSerializer responseSerializer;
            private final @NotNull FileInputStream input;
            private final int offset;

            private FileReader(@NotNull File file, int offset, @NotNull ResponseSerializer responseSerializer) throws FileNotFoundException {
                this.responseSerializer = responseSerializer;
                input = new FileInputStream(file);
                this.offset = offset * Constants.FILE_PART_SIZE;
            }

            @Override
            public void run() {
                try {
                    byte[] res = new byte[Constants.FILE_PART_SIZE];
                    int size = input.read(res, offset, res.length);
                    responseThreadPool.submit(new ResponseWorker(res, responseSerializer));
                } catch (IOException e) {
                    // TODO
                    e.printStackTrace();
                }
            }
        }

        private class ResponseWorker implements Runnable {
            private final @NotNull ResponseSerializer responseSerializer;
            private final @NotNull byte[] content;

            private ResponseWorker(@NotNull byte[] content, @NotNull ResponseSerializer responseSerializer) {
                this.content = content;
                this.responseSerializer = responseSerializer;
            }

            @Override
            public void run() {
                try {
                    GetResponse response = new GetResponse(content);
                    responseSerializer.write(response);
                } catch (SerializationException e) {
                    // TODO
                    e.printStackTrace();
                }
            }
        }

        private class RequestWorker implements Runnable, RequestVisitor {
            private final @NotNull RequestDeserializer requestDeserializer;
            private final @NotNull ResponseSerializer responseSerializer;

            private RequestWorker(@NotNull Socket socket) throws IOException {
                requestDeserializer = RequestDeserializer.createClientClientDeserializer(socket.getInputStream());
                responseSerializer = ResponseSerializer.createSerializer(socket.getOutputStream());
            }

            @Override
            public void run() {
                try {
                    AbstractRequest request = requestDeserializer.parseRequest();
                    request.accept(this);
                } catch (SerializationException e) {
                    // TODO
                    e.printStackTrace();
                }
            }

            @Override
            public void visitGetRequest(GetRequest request) {
                int id = request.getId();
                int part = request.getPart();
                File file = downloadedFiles.get(id);
                if (file != null) {
                    try {
                        fileReader.submit(new FileReader(file, part, responseSerializer));
                    } catch (FileNotFoundException e) {
                        // TODO
                        e.printStackTrace();
                    }
                } else {
                    System.out.println("holy shit");
                }
            }

            @Override
            public void visitStatRequest(StatRequest request) {
                ConcurrentHashMap<Integer, FilePartStatus> map = filesMap.get(request.getId());
                List<Integer> result;
                if (map != null) {
                    result = map.entrySet().stream()
                            .filter(entry -> entry.getValue() == FilePartStatus.SEEDING)
                            .map(Map.Entry::getKey)
                            .collect(Collectors.toList());
                } else {
                    result = Collections.emptyList();
                }
                StatResponse response = new StatResponse(result.size(), result);
                try {
                    responseSerializer.write(response);
                } catch (SerializationException e) {
                    // TODO
                    e.printStackTrace();
                }
            }

            @Override
            public void visitListRequest(ListRequest request) {
                throw new IllegalStateException();
            }

            @Override
            public void visitSourcesRequest(SourcesRequest request) {
                throw new IllegalStateException();
            }

            @Override
            public void visitUpdateRequest(UpdateRequest request) {
                throw new IllegalStateException();
            }

            @Override
            public void visitUploadRequest(UploadRequest request) {
                throw new IllegalStateException();
            }
        }
    }

    private class Downloader implements Runnable {
        private final @NotNull Random random = new Random();
        private final int id;
        private final int offset;
        private final @NotNull ListResponse.FileInfo fileInfo;
        private final @NotNull ConcurrentHashMap<Integer, FilePartStatus> fileMap;
        private final @NotNull File file;

        Downloader(int id, int offset, @NotNull ListResponse.FileInfo fileInfo) {
            this.id = id;
            this.offset = offset;
            this.fileInfo = fileInfo;
            fileMap = filesMap.computeIfAbsent(id, fileId -> fillFilePartMap(this.fileInfo.getSize(), FilePartStatus.NOT_LOADED));
            file = downloadedFiles.computeIfAbsent(id, fileId -> new File(DESTINATION_FOLDER + this.fileInfo.getName()));
        }

        @Override
        public void run() {
            List<List<InetSocketAddress>> addressesByOffset = sourcesMap.get(id);
            BooleanHolder shouldExit = new BooleanHolder(false);
            fileMap.compute(offset, (integer, filePartStatus) -> {
                if (filePartStatus != FilePartStatus.NOT_LOADED) {
                    shouldExit.setValue(true);
                    return filePartStatus;
                }
                return FilePartStatus.DOWNLOADING;
            });
            if (shouldExit.getValue()) {
                return;
            }
            List<InetSocketAddress> addresses = addressesByOffset.get(offset);
            if (addresses.isEmpty()) {
                fileMap.put(offset, FilePartStatus.NOT_LOADED);
                throw new RuntimeException();
            }
            InetSocketAddress address = addresses.get(random.nextInt(addresses.size()));
            try {
                AsynchronousSocketChannel channel = AsynchronousSocketChannel.open(clientsChannelGroup);
                channel.connect(address);

                ByteArrayOutputStream byteOutputStream = new ByteArrayOutputStream();
                RequestSerializer.createSerializer(byteOutputStream).write(new GetRequest(id, offset));
                channel.write(ByteBuffer.wrap(byteOutputStream.toByteArray()));

                ByteBuffer buffer = ByteBuffer.allocate(Constants.FILE_PART_SIZE);
                channel.read(buffer, null, new CompletionHandler<Integer, Object>() {
                    @Override
                    public void completed(Integer result, Object attachment) {
                        buffer.flip();
                        int size = offset == fileInfo.getNumberOfParts() - 1
                                ? (int) fileInfo.getSize() % Constants.FILE_PART_SIZE
                                : buffer.limit();
                        try {
                            OutputStream outputStream = new FileOutputStream(file);
                            outputStream.write(buffer.array(), offset, size);
                            fileMap.put(offset, FilePartStatus.SEEDING);
                        } catch (IOException e) {
                            fileMap.put(offset, FilePartStatus.NOT_LOADED);
                            e.printStackTrace();
                            downloaderThreadPool.submit(new Downloader(id, offset, fileInfo));
                        }
                    }

                    @Override
                    public void failed(Throwable exc, Object attachment) {
                        exc.printStackTrace();
                        downloaderThreadPool.submit(new Downloader(id, offset, fileInfo));
                    }
                });
            } catch (IOException e) {
                fileMap.put(offset, FilePartStatus.NOT_LOADED);
                downloaderThreadPool.submit(new Downloader(id, offset, fileInfo));
            }
        }
    }

    private abstract class CommandWithId implements AbstractCommand {
        abstract void processCommand(int id) throws CommandException;

        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {
            if (args.isEmpty()) {
                throw new CommandException("Missing argument <id>");
            }
            String id = args.get(0);
            try {
                processCommand(Integer.parseInt(id));
            } catch (NumberFormatException e) {
                throw new CommandException(id + " is not a number");
            }
        }
    }

    @Command("download")
    private class DownloadCommand extends CommandWithId {
        @Override
        public void processCommand(int id) throws CommandException {
            new SourcesCommand().processCommand(id);
            ListResponse.FileInfo fileInfo = filesOnServer.get(id);
            if (fileInfo == null) {
                throw new CommandException("Unknown id: " + id);
            }
            for (int offset = 0; (long) offset * Constants.FILE_PART_SIZE < fileInfo.getNumberOfParts(); offset++) {
                downloaderThreadPool.submit(new Downloader(id, offset, fileInfo));
            }
        }

        @Override
        public String getInfo() {
            return "download <id> -- download file by id";
        }
    }

    @Command("exit")
    private class ExitCommand implements AbstractCommand {
        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {
            working = false;
        }

        @Override
        public String getInfo() {
            return "exit -- exit application";
        }
    }

    @Command("list")
    private class ListCommand implements AbstractCommand {
        void processCommand() throws CommandException {
            try {
                Socket socket = createServerConnection();
                ListResponse response = CommunicationUtil.sendRequest(socket, new ListRequest());
                Map<Integer, ListResponse.FileInfo> newFilesOnServer = new HashMap<>();
                for (ListResponse.FileInfo fileInfo : response.getFileInfos()) {
                    newFilesOnServer.put(fileInfo.getId(), fileInfo);

                    System.out.println("File: " + fileInfo.getName());
                    System.out.println("Id: " + fileInfo.getId());
                    System.out.println("Size: " + fileInfo.getSizeInMb() + "Mb");
                    System.out.println();
                }
                filesOnServer = newFilesOnServer;
            } catch (IOException e) {
                throw new CommandException(e);
            }
        }

        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {
            processCommand();
        }

        @Override
        public String getInfo() {
            return "list -- get list of all available files";
        }
    }

    @Command("sources")
    private class SourcesCommand extends CommandWithId {
        @Override
        void processCommand(int id) throws CommandException {
            try {
                Socket socket = createServerConnection();
                SourcesRequest request = new SourcesRequest(id);
                SourcesResponse response = CommunicationUtil.sendRequest(socket, request);

                ListResponse.FileInfo fileInfo = filesOnServer.get(id);
                if (fileInfo == null) {
                    throw new CommandException("Unknown file: " + id);
                }
                List<List<InetSocketAddress>> sources = IntStream.range(0, fileInfo.getNumberOfParts())
                        .mapToObj(value -> new ArrayList<InetSocketAddress>())
                        .collect(Collectors.toList());

                for (InetSocketAddress clientAddress : response.getClients()) {
                    try {
                        Socket clientSocket = new Socket(clientAddress.getAddress(), clientAddress.getPort());
                        StatResponse statResponse = CommunicationUtil.sendRequest(clientSocket, new StatRequest(id));
                        for (Integer offset : statResponse.getParts()) {
                            List<InetSocketAddress> addresses = sources.get(offset);
                            addresses.add(clientAddress);
                        }
                    } catch (IOException e) {
                        // TODO: add logging
                        e.printStackTrace();
                    }
                }
                sourcesMap.put(id, sources);
            } catch (IOException e) {
                throw new CommandException(e);
            }
        }

        @Override
        public String getInfo() {
            return "sources <id> -- get sources of file <id>";
        }
    }

    @Command("upload")
    private class UploadCommand implements AbstractCommand {
        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {
            if (args.isEmpty()) throw new CommandException("No filename");
            String filename = args.get(0);
            File file = new File(DESTINATION_FOLDER + filename);
            if (!file.exists()) throw new CommandException("File " + filename + " not founded");
            long size = file.length();
            UploadRequest request = new UploadRequest(filename, size);
            try {
                Socket socket = createServerConnection();
                UploadResponse response = CommunicationUtil.sendRequest(socket, request);
                int id = response.getId();
                filesMap.put(id, fillFilePartMap(size, FilePartStatus.SEEDING));
                downloadedFiles.put(id, file);
            } catch (IOException e) {
                throw new CommandException(e);
            }
            new UpdateCommand().processCommand();
        }

        @Override
        public String getInfo() {
            return "upload <file> -- upload file to tracker";
        }
    }

    @Command("update")
    private class UpdateCommand implements AbstractCommand {
        void processCommand() throws CommandException {
            try (Socket socket = createServerConnection()) {
                List<Integer> seedingFiles = filesMap.entrySet().stream()
                        .filter(entry -> entry.getValue().values().stream()
                                .anyMatch(filePartStatus -> filePartStatus == FilePartStatus.SEEDING))
                        .map(Map.Entry::getKey)
                        .collect(Collectors.toList());
                UpdateRequest request = new UpdateRequest(server.getServerPort(), seedingFiles);
                UpdateResponse response = CommunicationUtil.sendRequest(socket, request);
                String status = response.getStatus() ? "OK" : "Fail";
                System.out.println("Status: " + status);
            } catch (IOException e) {
                throw new CommandException(e);
            }
        }

        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {
            processCommand();
        }

        @Override
        public String getInfo() {
            return "update -- update list of available seeds";
        }
    }

    @Command("help")
    private class HelpCommand implements AbstractCommand {
        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {
            commands.keySet().stream().sorted().forEach(command -> System.out.println(commands.get(command).getInfo()));
        }

        @Override
        public String getInfo() {
            return "help -- print this help";
        }
    }

    @Command("status")
    // TODO
    private class Status implements AbstractCommand {
        @Override
        public void processCommand(@NotNull List<@NotNull String> args) throws CommandException {

        }

        @Override
        public String getInfo() {
            return "status -- print status about active files";
        }
    }

    @NotNull
    private static ConcurrentHashMap<Integer, FilePartStatus> fillFilePartMap(long size, FilePartStatus value) {
        Map<Integer, FilePartStatus> fileMap = new HashMap<>();
        for (int offset = 0; (long) offset * Constants.FILE_PART_SIZE < size; offset++) {
            fileMap.put(offset, value);
        }
        return new ConcurrentHashMap<>(fileMap);
    }
}