package itmo2018.se;

import itmo2018.se.SingletonFileReader.FileHolder;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;

public class Seeder implements Runnable, Closeable {
    private ServerSocket server;
    private MetaDataManager metaData;
    private ThreadPoolExecutor poolExecutor;
    private Client client;
    private int limitLeech = 4;

    public Seeder(ServerSocket server, MetaDataManager metaData, Client client) {
        this.server = server;
        this.metaData = metaData;
        this.poolExecutor = (ThreadPoolExecutor) Executors.newFixedThreadPool(limitLeech);
        this.client = client;
    }

    @Override
    public void run() {
        while (true) {
            try {
                Socket socket = server.accept();
                if (poolExecutor.getActiveCount() >= limitLeech) {
                    socket.close();
                    continue;
                }
                poolExecutor.submit(new Executor(socket));
            } catch (IOException e) {
                break;
            }
        }
    }

    @Override
    public void close() throws IOException {
        server.close();
        poolExecutor.shutdownNow();
    }

    private class Executor implements Runnable {
        Socket socket;
        FileHolder file = null;

        Executor(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            while (true) {
                try (DataInputStream socketIn = new DataInputStream(socket.getInputStream());
                     DataOutputStream socketOut = new DataOutputStream(socket.getOutputStream())) {
                    while (true) {
                        byte cmd = socketIn.readByte();
                        if (cmd == 1) {
                            execStat(socketIn, socketOut);
                        } else {
                            execGet(socketIn, socketOut);
                        }
                    }
                } catch (IOException e) {
                    //disconnect leech
                }
                try {
                    socket.close();
                    if (file != null) {
                        file.close();
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                return;
            }
        }

        private void execStat(DataInputStream socketIn, DataOutputStream socketOut) throws IOException {
            int id = socketIn.readInt();
            MetaDataNote note = metaData.getNote(id);
            if (note == null) {
                socketOut.writeInt(0);
            } else if (!note.existFile()) {
                metaData.deleteNote(id);
                socketOut.writeInt(0);
            } else {
                socketOut.writeInt(note.partsCount());
                for (int part : note) {
                    socketOut.writeInt(part);
                }
            }
            socketOut.flush();
        }

        private void execGet(DataInputStream socketIn, DataOutputStream socketOut) throws IOException {
            int id = socketIn.readInt();
            int part = socketIn.readInt();

            MetaDataNote note = metaData.getNote(id);
            if (file == null) {
                File neededFile = new File(note.getName());
                if (!neededFile.exists()) {
                    metaData.deleteNote(id);
                    client.update((short) server.getLocalPort());
                    socketOut.writeInt(0);
                    socketOut.flush();
                    socket.close();
                    return;
                }
                file = SingletonFileReader.getFile(neededFile);
            }
            long defaultPartSize = 1024 * 1024 * 5;
            int partSize = (int) (file.length() - defaultPartSize * part > defaultPartSize ?
                    defaultPartSize : file.length() - defaultPartSize * part);
            byte[] bytes = new byte[partSize];
            file.read(defaultPartSize * part, bytes);

            socketOut.writeInt(partSize);
            socketOut.write(bytes);
            socketOut.flush();
        }
    }
}
