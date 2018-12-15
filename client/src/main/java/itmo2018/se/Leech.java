package itmo2018.se;

import org.apache.commons.collections4.SetUtils;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.function.Function;
import java.util.stream.Collectors;

public class Leech implements Runnable {
    private MetaDataManager metaData;
    private int fileId;
    private Client client;
    private ThreadPoolExecutor downloadPool = (ThreadPoolExecutor) Executors.newFixedThreadPool(8);
    private final Object mutex = new Object();
    private short ownSeederPort;

    public Leech(int fileId, Client client, MetaDataManager metaData, short ownSeederPort) {
        this.fileId = fileId;
        this.client = client;
        this.metaData = metaData;
        this.ownSeederPort = ownSeederPort;
    }

    @Override
    public void run() {
        FileWriter writer = null;
        try {
            MetaDataNote note = metaData.getNote(fileId);
            if (note != null && !note.existFile()) {
                metaData.deleteNote(fileId);
                System.out.println("name: " + note.getName());
            }
            List<InetSocketAddress> owners = client.getSources(fileId);
            if (owners.size() == 0) {
                System.out.println("no active seeders, please try later");
                return;
            }

            MetaDataNote fileInfo = client.getList().get(fileId);
            Set<Integer> parts;
            if (note == null) {
                metaData.addNote(fileInfo.getId(), fileInfo.getName(), fileInfo.getSize());
                parts = new HashSet<>();
            } else {
                parts = note.getParts();
            }

            Map<Integer, PartInfo> neededParts = SetUtils.difference(fileInfo.getParts(), parts)
                    .stream().collect(Collectors.toConcurrentMap(Function.identity(),
                            it -> new PartInfo(it, DownloadStatus.WAITING)));

            writer = new FileWriter(fileInfo);
            writer.start();

            int finish;
            do {
                for (InetSocketAddress owner : owners) {
                    downloadPool.submit(new Downloader(owner, fileInfo, neededParts, writer));
                }
                Thread.sleep(20000);
                finish = (int) neededParts.values().stream().filter(it -> it.status == DownloadStatus.FINISH).count();
                owners = client.getSources(fileId);
                System.out.println("=================================================");
                System.out.println("active count: " + downloadPool.getActiveCount());
                System.out.println("finish: " + finish + "\t" + "neededSize: " + neededParts.size());
                System.out.println("=================================================");
            } while (downloadPool.getActiveCount() > 0 && finish < neededParts.size());

            if (finish == neededParts.size()) {
                metaData.finishCollectParts(fileId);
                System.out.println("finish download file " + fileInfo.getName());
            } else {
                System.out.println("not possible to download the whole file because there are no seeders");
            }
            client.update(ownSeederPort);
        } catch (IOException e) {
            System.out.println("can't connect to seeder");
        } catch (InterruptedException e) {
            e.printStackTrace();
        } finally {
            downloadPool.shutdown();
            if (writer != null) {
                writer.interrupt();
            }
        }
    }

    private class Downloader implements Runnable {
        InetSocketAddress address;
        final Map<Integer, PartInfo> neededParts;
        MetaDataNote fileInfo;
        FileWriter writer;

        Downloader(InetSocketAddress address, MetaDataNote fileInfo, Map<Integer, PartInfo> neededParts,
                   FileWriter writer) {
            this.address = address;
            this.neededParts = neededParts;
            this.fileInfo = fileInfo;
            this.writer = writer;
        }

        @Override
        public void run() {
            try (Socket socket = new Socket()) {
                socket.connect(address);
                Set<Integer> userParts = getStat(socket);

                for (Map.Entry<Integer, PartInfo> neededPart : neededParts.entrySet()) {
                    int partNumber = neededPart.getKey();
                    if (!userParts.contains(partNumber)) {
                        continue;
                    }
                    PartInfo partInfo = neededPart.getValue();
                    if (partInfo.status == DownloadStatus.WAITING) {
                        synchronized (neededParts) {
                            partInfo = neededParts.get(partNumber);
                            if (partInfo.status == DownloadStatus.WAITING) {
                                partInfo.status = DownloadStatus.IN_PROGRESS;
                            } else {
                                continue;
                            }
                        }
                        try {
                            byte[] bytes = download(socket, partNumber);
                            System.out.println("download part " + partNumber);
                            partInfo.content = bytes;
                            writer.addPart(partInfo);
                        } catch (IOException e) {
                            partInfo.status = DownloadStatus.WAITING;
                            return;
                        }
                    }
                }
            } catch (IOException e) {
                System.out.println("seeder " + address + " is busy");
            } finally {
                synchronized (mutex) {
                    mutex.notify();
                }
            }
        }

        private byte[] download(Socket socket, int part) throws IOException {
            DataInputStream socketIn = new DataInputStream(socket.getInputStream());
            DataOutputStream socketOut = new DataOutputStream(socket.getOutputStream());
            socketOut.writeByte(2);
            socketOut.writeInt(fileInfo.getId());
            socketOut.writeInt(part);
            socketOut.flush();

            int partSize = socketIn.readInt();
            byte[] bytes = new byte[partSize];
            for (int i = 0; i < partSize; i++) {
                bytes[i] = socketIn.readByte();
            }
            return bytes;
        }

        private Set<Integer> getStat(Socket socket) throws IOException {
            DataInputStream socketIn = new DataInputStream(socket.getInputStream());
            DataOutputStream socketOut = new DataOutputStream(socket.getOutputStream());
            socketOut.writeByte(1);
            socketOut.writeInt(fileId);
            socketOut.flush();

            int partsCount = socketIn.readInt();
            Set<Integer> result = new HashSet<>();
            for (int i = 0; i < partsCount; i++) {
                int part = socketIn.readInt();
                result.add(part);
            }
            return result;
        }
    }

    private class FileWriter extends Thread {
        MetaDataNote fileInfo;
        BlockingQueue<PartInfo> queue = new LinkedBlockingQueue<>();

        FileWriter(MetaDataNote fileInfo) {
            this.fileInfo = fileInfo;
        }

        void addPart(PartInfo partInfo) {
            try {
                queue.put(partInfo);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void run() {
            int partSize = 1024 * 1024 * 5;
            File file = new File(fileInfo.getName());
            if (!file.exists()) {
                try {
                    file.createNewFile();
                } catch (IOException e) {
                    System.out.println("file could not be created");
                }
            }
            try (RandomAccessFile accessfile = new RandomAccessFile(file, "rw")) {
                while (!this.isInterrupted() || queue.size() > 0) {
                    //TODO переделать queue
                    PartInfo partInfo = queue.take();

                    accessfile.seek(partSize * partInfo.number);
                    accessfile.write(partInfo.content);

                    metaData.addPart(fileInfo.getId(), partInfo.number);
                    partInfo.status = DownloadStatus.FINISH;
                }
            } catch (IOException e) {
                e.printStackTrace();
            } catch (InterruptedException e) {
                //return from writer
            }
        }
    }

    private enum DownloadStatus {
        WAITING, IN_PROGRESS, FINISH
    }

    private class PartInfo {
        int number;
        volatile byte[] content;
        volatile DownloadStatus status;

        PartInfo(int number, DownloadStatus status) {
            this.number = number;
            this.status = status;
        }
    }
}
