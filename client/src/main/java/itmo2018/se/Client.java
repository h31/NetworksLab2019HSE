package itmo2018.se;

import java.io.*;
import java.net.Socket;
import java.util.Scanner;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;


public class Client {
    public static void main(String[] args) throws IOException, InterruptedException {
        File workingDir;
        if (args.length == 0) {
            workingDir = new File(System.getProperty("user.dir"));
        } else {
            workingDir = new File(args[0]);
            if (!workingDir.exists() && !workingDir.isDirectory()) {
                System.out.println("can't find " + args[0] + " folder");
                return;
            }
        }
        new Client().run(workingDir.getAbsolutePath());
    }

    public void run(String workingDir) throws IOException, InterruptedException {
        initWorkingDir(workingDir);

        Socket socket = new Socket("127.0.0.1", 8081);
        final DataOutputStream out = new DataOutputStream(socket.getOutputStream());
        final DataInputStream in = new DataInputStream(socket.getInputStream());
        Updater updater = new Updater(workingDir, in, out);
        ScheduledExecutorService scheduled = Executors.newScheduledThreadPool(1);
        scheduled.scheduleAtFixedRate(updater, 0, 5, TimeUnit.MINUTES);


        Scanner scanner = new Scanner(System.in);

        while (true) {
            String[] cmdLine = scanner.nextLine().split(" +");
            switch (cmdLine[0]) {
                case "list":
                    if (cmdLine.length != 1) {
                        System.out.println("list not takes arguments");
                        continue;
                    }
                    sendList(in, out);
                    break;
                case "upload":
                    if (cmdLine.length != 2) {
                        System.out.println("upload takes one argument");
                        continue;
                    }
                    sendUpload(cmdLine[1], workingDir, in, out);
                    break;
                case "source":
                    if (cmdLine.length != 2) {
                        System.out.println("upload takes one argument");
                        continue;
                    } else if (!isPositiveInt(cmdLine[1])) {
                        System.out.println("id must be positive integer number");
                        continue;
                    }
                    sendSources(Integer.parseInt(cmdLine[1]), in, out);
                    break;
                case "exit":
                    socket.close();
                    scheduled.shutdownNow();
                    return;
                default:
                    System.out.println(cmdLine[0] + " is unknow command");
            }

        }

//        ByteArrayOutputStream byteOutput = new ByteArrayOutputStream();
//        DataOutputStream byteDataOut = new DataOutputStream(byteOutput);
////        InputStream in = socket.getInputStream();
//        Thread.sleep(4000);
//        //upload
//        byteDataOut.writeByte(2);
//        byteDataOut.writeUTF("name");
//        byteDataOut.writeLong(191);
//        out.writeInt(byteDataOut.size());
//        out.write(byteOutput.toByteArray());
//        out.flush();
//        //===============
//        System.out.println(in.readInt());
        //update
//        out.writeInt(7);
//        out.writeByte(4);
//        out.writeShort(0);
//        out.writeInt(0);
//        //=================
//
//        //sources
//        out.writeInt(5);
//        out.writeByte(3);
//        out.writeInt(0);
//        out.flush();
//        //=================
//
//        Thread.sleep(4000);
//        //update
//        out.writeInt(7);
//        out.writeByte(4);
//        out.writeShort(0);
//        out.writeInt(0);
//        out.flush();
//        //=============
//
//        //list
//        out.writeInt(1);
//        out.writeByte(1);
//        //==============
//        Thread.sleep(15000);

//        socket.close();
    }

    private void initWorkingDir(String workingDir) throws IOException {
        System.out.println("working dir:\t" + workingDir);
        File metaData = new File(workingDir + "/.metadata");
        if (!metaData.exists()) {
            metaData.createNewFile();
        }
    }


    private void sendSources(int id, DataInputStream socketIn, DataOutputStream socketOut) throws IOException {
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
                short port = socketIn.readShort();
                System.out.println("ip: " + ip1 + "." + ip2 + "." + ip3 + "." + ip4 +
                        "\t port: " + port);
            }
        }
    }

    private void sendList(DataInputStream socketIn, DataOutputStream socketOut) throws IOException {
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

    private void sendUpload(String filePath, String workingDir, DataInputStream socketIn, DataOutputStream socketOut) throws IOException {
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

            try (OutputStream metaData = new FileOutputStream(workingDir + "/.metadata")) {
                metaData.write((id + "\t" + file.getAbsolutePath() + "\t" + -1 + "\n").getBytes());
            }
            System.out.println("new file id: " + id);
        }
    }

    private boolean isPositiveInt(String str) {
        return str.matches("\\d+");
    }
}

