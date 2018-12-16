package itmo2018.se;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class Client implements Closeable {
    private MetaDataManager metaData;
    private Socket socket;
    private DataOutputStream socketOut;
    private DataInputStream socketIn;
    private ScheduledExecutorService scheduled;

    //"192.168.1.2"
    public Client(String host, MetaDataManager metaData) throws IOException {
        this.socket = new Socket(host, 8081);
        this.metaData = metaData;
        try {
            socketOut = new DataOutputStream(socket.getOutputStream());
            socketIn = new DataInputStream(socket.getInputStream());
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public List<InetSocketAddress> getSources(int id) throws IOException {
        synchronized (socketOut) {
            socketOut.writeInt(1 + 4);
            socketOut.writeByte(3);
            socketOut.writeInt(id);

            int size = socketIn.readInt();
            List<InetSocketAddress> res = new ArrayList<>(size);
            for (int i = 0; i < size; i++) {
                int ip1 = byteToInt(socketIn.readByte());
                int ip2 = byteToInt(socketIn.readByte());
                int ip3 = byteToInt(socketIn.readByte());
                int ip4 = byteToInt(socketIn.readByte());
                int port = shortToInt(socketIn.readShort());
                InetSocketAddress address = new InetSocketAddress(ip1 + "." + ip2 + "." + ip3 + "." + ip4, port);
                res.add(address);
            }
            return res;
        }
    }

    public void printSource(int id) throws IOException {
        List<InetSocketAddress> adresses = getSources(id);
        for (InetSocketAddress address : adresses) {
            System.out.println(address);
        }
    }

    public void printList() throws IOException {
        List<MetaDataNote> notes = getList();
        for (MetaDataNote note : notes) {
            System.out.println(note.getId() + "\t" + note.getName() + "\t" + note.getSize());
        }
    }

    public List<MetaDataNote> getList() throws IOException {
        synchronized (socketOut) {
            socketOut.writeInt(1);
            socketOut.writeByte(1);

            int count = socketIn.readInt();
            List<MetaDataNote> result = new ArrayList<>();
            for (int i = 0; i < count; i++) {
                int fileId = socketIn.readInt();
                String fileName = socketIn.readUTF();
                long fileSize = socketIn.readLong();
                result.add(new MetaDataNote(fileId, fileName, fileSize));
            }
            return result;
        }
    }

    public void upload(String filePath) throws IOException {
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
            metaData.addReadyNote(id, file.getAbsolutePath(), file.length());
            System.out.println("new file id: " + id);
        }
    }

    public void update(short seederPort) throws IOException {
        synchronized (socketOut) {
            int filesCount = metaData.filesCount();
            socketOut.writeInt(1 + 2 + (filesCount + 1) * 4);
            socketOut.writeByte(4);
            socketOut.writeShort(seederPort);
            socketOut.writeInt(filesCount);
            for (int id : metaData.idSet()) {
                socketOut.writeInt(id);
            }
            socketOut.flush();
            socketIn.readByte();
        }
    }

    public void startUpdat(short seedPort) {
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
        if (scheduled != null) {
            scheduled.shutdownNow();
        }
    }

    private int byteToInt(byte b) {
        if (b >= 0) {
            return b;
        }
        return 128 + 128 + b;
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
                update(seederPort);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
