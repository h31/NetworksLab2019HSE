package itmo2018.se;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadPoolExecutor;

public class Seeder implements Runnable, Closeable {
    private ServerSocket server;
    private MetaDataManager metaData;
    private ThreadPoolExecutor poolExecutor;
    private int limitLeech = 4;

    public Seeder(ServerSocket server, MetaDataManager metaData) {
        this.server = server;
        this.metaData = metaData;
        poolExecutor = (ThreadPoolExecutor) Executors.newFixedThreadPool(limitLeech);
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
        RandomAccessFile file = null;

        Executor(Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            System.out.println("run seeder");
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
                    System.out.println("leech disconected");
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

            System.out.println("needed part: " + part);

            MetaDataNote note = metaData.getNote(id);
            if (file == null) {
                file = new RandomAccessFile(note.getName(), "r");
            }
            long defaultPartSize = 1024 * 1024 * 5;
            file.seek(defaultPartSize * part);
            int partSize = (int) (file.length() - defaultPartSize * part > defaultPartSize ?
                    defaultPartSize : file.length() - defaultPartSize * part);
            byte[] bytes = new byte[partSize];
             file.read(bytes);

            socketOut.writeInt(partSize);
            socketOut.write(bytes);
            socketOut.flush();
        }
    }
}
