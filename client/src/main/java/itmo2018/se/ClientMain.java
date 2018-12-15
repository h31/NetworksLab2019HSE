package itmo2018.se;

import java.io.*;
import java.net.ServerSocket;
import java.util.Scanner;

public class ClientMain {
    public static void main(String[] args) throws IOException {
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
        new ClientMain().run(workingDir.getAbsolutePath());
    }

    public void run(String workingDir) throws IOException {
        MetaDataManager metaData = initWorkingDir(workingDir);

        Client client;
        try {
            client = new Client(metaData);
        } catch (IOException e) {
            System.out.println("can't connect to server");
            return;
        }
        ServerSocket seederServer = new ServerSocket();
        seederServer.bind(client.getSocketLocalAdress());
        seederServer.setReceiveBufferSize(1024);

        short seedPort = (short) seederServer.getLocalPort();
        System.out.println("port: " + seederServer.getLocalPort());
        client.startUpdat(seedPort);

        Seeder seeder = new Seeder(seederServer, metaData);
        Thread seederThread = new Thread(seeder);
        seederThread.setDaemon(true);
        seederThread.start();

        Scanner scanner = new Scanner(System.in);
        while (true) {
            String[] cmdLine = scanner.nextLine().split(" +");
            try {
                switch (cmdLine[0]) {
                    case "download":
                        if (cmdLine.length != 2) {
                            System.out.println("download takes one argument");
                            continue;
                        } else if (!isPositiveInt(cmdLine[1])) {
                            System.out.println("id must be positive integer number");
                            continue;
                        }
                        //TODO добавить пул личей
                        new Leech(Integer.parseInt(cmdLine[1]), client, metaData).run();
                        break;
                    case "list":
                        if (cmdLine.length != 1) {
                            System.out.println("list not takes arguments");
                            continue;
                        }
                        client.getList();
                        break;
                    case "upload":
                        if (cmdLine.length != 2) {
                            System.out.println("upload takes one argument");
                            continue;
                        }
                        client.upload(cmdLine[1]);
                        break;
                    case "source":
                        if (cmdLine.length != 2) {
                            System.out.println("source takes one argument");
                            continue;
                        } else if (!isPositiveInt(cmdLine[1])) {
                            System.out.println("id must be positive integer number");
                            continue;
                        }
                        client.getSources(Integer.parseInt(cmdLine[1]));
                        break;
                    case "update":
                        if (cmdLine.length != 1) {
                            System.out.println("update not takes arguments");
                            continue;
                        }
                        client.update(seedPort);
                        break;
                    case "exit":
                        if (cmdLine.length != 1) {
                            System.out.println("exit not takes arguments");
                            continue;
                        }
                        client.close();
                        seeder.close();
                        return;
                    default:
                        System.out.println(cmdLine[0] + " is unknow command");
                }
            } catch (IOException e) {
                System.out.println("connection aborted");
                client.close();
                seeder.close();
                return;
            }
        }
    }

    private MetaDataManager initWorkingDir(String workingDir) throws IOException {
        System.out.println("working dir:\t" + workingDir);
        File metaData = new File(workingDir + "/.metadata");
        if (!metaData.exists()) {
            metaData.createNewFile();
        }
        return new MetaDataManager(metaData);
    }

    private boolean isPositiveInt(String str) {
        return str.matches("\\d+");
    }
}

