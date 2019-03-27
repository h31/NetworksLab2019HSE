package ru.hse.spb.server;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.Constants;
import ru.hse.spb.protocol.deserialization.RequestDeserializer;
import ru.hse.spb.protocol.request.*;
import ru.hse.spb.protocol.response.ListResponse;
import ru.hse.spb.protocol.response.SourcesResponse;
import ru.hse.spb.protocol.response.UpdateResponse;
import ru.hse.spb.protocol.response.UploadResponse;
import ru.hse.spb.protocol.serialization.ResponseSerializer;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class Server {
    private static final int THREAD_POOL_SIZE = 4;
    private static final int PARALLELISM_THRESHOLD = 4;
    private static final int DROP_DELAY = 5; // in minutes

    private static final @NotNull Comparator<InetSocketAddress> addressComparator = Comparator.comparing(InetSocketAddress::getHostString);

    private final @NotNull AtomicInteger idCounter = new AtomicInteger();

    // <file id, file info>
    private final ConcurrentHashMap<Integer, ListResponse.FileInfo> filesMap = new ConcurrentHashMap<>();
    // <file id, set of seed addresses>
    private final ConcurrentHashMap<Integer, ConcurrentSkipListSet<InetSocketAddress>> clientsMap = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<InetSocketAddress, ScheduledFuture<?>> cancelClientMap = new ConcurrentHashMap<>();

    private final ScheduledExecutorService clientDropper = Executors.newSingleThreadScheduledExecutor();
    private final ExecutorService threadPool = Executors.newFixedThreadPool(THREAD_POOL_SIZE);

    public void serve() throws IOException {
        ServerSocket serverSocket = new ServerSocket(Constants.SERVER_PORT);
        while (true) {
            try {
                Socket socket = serverSocket.accept();
                threadPool.submit(new ClientWorker(socket));
            } catch (IOException e) {
                handleException(e);
            }
        }
    }

    private void handleException(Throwable e) {
        System.err.println("Exception in client worker");
        e.printStackTrace();
    }

    private class ClientWorker implements Runnable, RequestVisitor {
        private final @NotNull Socket socket;
        private final @NotNull ResponseSerializer serializer;
        private final @NotNull RequestDeserializer deserializer;

        private ClientWorker(@NotNull Socket socket) throws IOException {
            this.socket = socket;
            serializer = ResponseSerializer.createSerializer(socket.getOutputStream());
            deserializer = RequestDeserializer.createClientServerDeserializer(socket.getInputStream());
        }

        @Override
        public void run() {
            try {
                AbstractRequest request = deserializer.parseRequest();
                request.accept(this);
                socket.close();
            } catch (Throwable e) {
                System.err.println("Exception in client worker");
                e.printStackTrace();
            }
        }

        @Override
        public void visitListRequest(ListRequest request) {
            try {
                List<ListResponse.FileInfo> fileInfos = new ArrayList<>(filesMap.values());
                ListResponse response = new ListResponse(fileInfos);
                serializer.write(response);
            } catch (Throwable e) {
                handleException(e);
            }
        }

        @Override
        public void visitSourcesRequest(SourcesRequest request) {
            try {
                int fileId = request.getId();
                ConcurrentSkipListSet<InetSocketAddress> addresses = clientsMap.get(fileId);
                SourcesResponse response = new SourcesResponse(addresses == null ? Collections.emptyList() : new ArrayList<>(addresses));
                serializer.write(response);
            } catch (Throwable e) {
                handleException(e);
            }
        }

        @Override
        public void visitUpdateRequest(UpdateRequest request) {
            try {
                InetSocketAddress seedAddress = new InetSocketAddress(socket.getInetAddress(), request.getClientPort());
                cancelClientMap.compute(seedAddress, (inetSocketAddress, scheduledFuture) -> {
                    if (scheduledFuture != null) {
                        scheduledFuture.cancel(false);
                    }
                    return clientDropper.schedule(new ClientDropper(seedAddress), DROP_DELAY, TimeUnit.MINUTES);
                });

                for (Integer id : request.getIds()) {
                    ConcurrentSkipListSet<InetSocketAddress> addresses = clientsMap.computeIfAbsent(id, integer -> new ConcurrentSkipListSet<>(addressComparator));
                    addresses.add(seedAddress);
                }
                UpdateResponse response = new UpdateResponse(true);
                serializer.write(response);
            } catch (Throwable e) {
                handleException(e);
            }
        }

        @Override
        public void visitUploadRequest(UploadRequest request) {
            try {
                int fileId = idCounter.getAndIncrement();
                ListResponse.FileInfo fileInfo = new ListResponse.FileInfo(fileId, request.getName(), request.getSize());
                filesMap.put(fileId, fileInfo);
                UploadResponse response = new UploadResponse(fileId);
                serializer.write(response);
            } catch (Throwable e) {
                handleException(e);
            }
        }

        @Override
        public void visitStatRequest(StatRequest request) {
            throw new IllegalStateException();
        }

        @Override
        public void visitGetRequest(GetRequest request) {
            throw new IllegalStateException();
        }
    }

    private class ClientDropper implements Runnable {
        private final @NotNull InetSocketAddress seedAddress;

        ClientDropper(@NotNull InetSocketAddress seedAddress) {
            this.seedAddress = seedAddress;
        }

        @Override
        public void run() {
            cancelClientMap.remove(seedAddress);
            clientsMap.forEachValue(PARALLELISM_THRESHOLD, addresses -> addresses.remove(seedAddress));
        }
    }
}