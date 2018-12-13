package itmo2018.se;

import java.io.*;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Client implements Closeable {
    private String metaData;
    private Socket socket;
    private DataOutputStream socketOut;
    private DataInputStream socketIn;
    private ScheduledExecutorService scheduled;


    public Client(String workingDir) throws IOException {
        this.socket = new Socket("localhost", 8081);
        this.metaData = workingDir + "/.metadata";
        try {
            socketOut = new DataOutputStream(socket.getOutputStream());
            socketIn = new DataInputStream(socket.getInputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void sendSources(int id) throws IOException {
        synchronized (socketOut) {
            socketOut.writeInt(1 + 4);
            socketOut.writeByte(3);
            socketOut.writeInt(id);

            int size = socketIn.readInt();
            for (int i = 0; i < size; i++) {
                byte ip1 = socketIn.readByte();
                byte ip2 = socketIn.readByte();
                byte ip3 = socketIn.readByte();
                byte ip4 = socketIn.readByte();
                int port = shortToInt(socketIn.readShort());
                System.out.println("ip: " + ip1 + "." + ip2 + "." + ip3 + "." + ip4 +
                        "\t port: " + port);
            }
        }
    }

    public void sendList() throws IOException {
        synchronized (socketOut) {
            socketOut.writeInt(1);
            socketOut.writeByte(1);

            int count = socketIn.readInt();
            for (int i = 0; i < count; i++) {
                int fileId = socketIn.readInt();
                String fileName = socketIn.readUTF();
                long fileSize = socketIn.readLong();
                System.out.println(fileId + "\t" + fileName + "\t" + fileSize);
            }
        }
    }

    public void sendUpload(String filePath) throws IOException {
        File file = new File(filePath);
        if (!file.exists() || file.isDirectory()) {
            System.out.println("can't find file");
            return;
        }
        String name = file.getName();
        long size = file.length();
        synchronized (socketOut) {
            socketOut.writeInt(1 + name.getBytes().length + 2 + 8);
            socketOut.writeByte(2);
            socketOut.writeUTF(name);
            socketOut.writeLong(size);
            socketOut.flush();

            int id = socketIn.readInt();

            try (OutputStream metaDataFile = new FileOutputStream(metaData)) {
                metaDataFile.write((id + "\t" + file.getAbsolutePath() + "\t" + -1 + "\n").getBytes());
            }
            System.out.println("new file id: " + id);
        }
    }

    public void sendUpdate(short seederPort) throws IOException {
        synchronized (socketOut) {
            int filesNumber = (int) Files.lines(Paths.get(metaData)).count();
            System.out.println("filesNumber: " + filesNumber);
            socketOut.writeInt(1 + 2 + (filesNumber + 1) * 4);
            socketOut.writeByte(4);
            socketOut.writeShort(seederPort);
            socketOut.writeInt(filesNumber);
            Files.lines(Paths.get(metaData)).map(it -> Integer.parseInt(it.split("\t")[0]))
                    .forEach(it -> {
                        try {
                            socketOut.writeInt(it);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    });
            socketOut.flush();
            socketIn.readByte();
        }
    }

    public void startUpdater(short seedPort) {
        Updater updater = new Updater(seedPort);
        this.scheduled = Executors.newSingleThreadScheduledExecutor();
        scheduled.scheduleAtFixedRate(updater, 0, 5, TimeUnit.MINUTES);
    }

    public SocketAddress getSocketLocalAdress() {
        return socket.getLocalSocketAddress();
    }


    @Override
    public void close() throws IOException {
        socket.close();
        scheduled.shutdownNow();
    }

    private int shortToInt(short s) {
        if (s >= 0) {
            return s;
        }
        return 32768 + 32768 + s;
    }

    private class Updater implements Runnable {
        short seederPort;

        Updater(short seederPort) {
            this.seederPort = seederPort;
        }

        @Override
        public void run() {
            try {
                sendUpdate(seederPort);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
