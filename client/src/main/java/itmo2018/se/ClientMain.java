package itmo2018.se;

import java.io.*;
import java.net.ServerSocket;
import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class ClientMain {
    public static void main(String[] args) throws IOException {
        File workingDir;
        if (args.length == 0) {
            System.out.println("lack of tracker host and working directory");
            return;
        } else if (args.length == 1) {
            workingDir = new File(System.getProperty("user.dir"));
        } else {
            workingDir = new File(args[1]);
            if (!workingDir.exists() && !workingDir.isDirectory()) {
                System.out.println("can't find " + args[1] + " folder");
                return;
            }
        }
        String host = args[0];
        new ClientMain().run(host, workingDir.getAbsolutePath());
    }

    public void run(String trackerHost, String workingDir) throws IOException {
        MetaDataManager metaData = initWorkingDir(workingDir);

        Client client;
        try {
            client = new Client(trackerHost, metaData);
        } catch (IOException e) {
            System.out.println("can't connect to server");
            return;
        }
        ServerSocket seederServer = new ServerSocket();
        seederServer.bind(client.getSocketLocalAdress());
        seederServer.setReceiveBufferSize(1024);

        short seedPort = (short) seederServer.getLocalPort();
        client.startUpdat(seedPort);

        Seeder seeder = new Seeder(seederServer, metaData, client);
        Thread seederThread = new Thread(seeder);
        seederThread.setDaemon(true);
        seederThread.start();

        ExecutorService leechPool = Executors.newFixedThreadPool(4);

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
                        leechPool.submit(new Leech(Integer.parseInt(cmdLine[1]), client, metaData, seedPort));
                        break;
                    case "list":
                        if (cmdLine.length != 1) {
                            System.out.println("list not takes arguments");
                            continue;
                        }
                        client.printList();
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
                        client.printSource(Integer.parseInt(cmdLine[1]));
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
                        leechPool.shutdownNow();
                        return;
                    default:
                        System.out.println(cmdLine[0] + " is unknow command");
                }
            } catch (IOException e) {
                System.out.println("connection aborted");
                client.close();
                seeder.close();
                leechPool.shutdown();
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

