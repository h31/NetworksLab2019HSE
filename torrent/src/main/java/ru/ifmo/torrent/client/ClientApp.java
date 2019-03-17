package ru.ifmo.torrent.client;

import ru.ifmo.torrent.client.storage.LocalFileReference;
import ru.ifmo.torrent.tracker.state.FileInfo;
import ru.ifmo.torrent.tracker.state.SeedInfo;
import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.net.InetAddress;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.InputMismatchException;
import java.util.List;
import java.util.Scanner;

public class ClientApp {

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        System.out.print("enter client port: ");
        Short port = getPort(args, scanner);

        try (Client client = new Client(InetAddress.getLocalHost(), port, ClientConfig.getMetaDir(), ClientConfig.TORRENT_DIR)) {
            System.out.printf("client started at getPort %d%n", port);
            printUsage();
            main_while:
            while (scanner.hasNext()) {
                String command = scanner.next();
                switch (command) {
                    case Command.EXIT: {
                        System.out.println("shutting down client");
                        break main_while;
                    }
                    case Command.LIST: {
                        List<FileInfo> files = client.getAvailableFiles();
                        System.out.printf("files count: %d%n", files.size());
                        for (FileInfo f : files) {
                            System.out.printf("\t%s\tid: %d, getSize: %d bytes%n",
                                f.getName(),
                                f.getId(),
                                f.getSize()
                            );
                        }
                        break;
                    }
                    case Command.UPLOAD: {
                        String path = scanner.next();
                        Path file = Paths.get(path);
                        int fileId = client.uploadFile(file);
                        System.out.printf("file added with id: %d%n", fileId);
                        break;
                    }
                    case Command.SOURCES: {
                        int fileId = scanner.nextInt();
                        List<SeedInfo> sources = client.getFileSources(fileId);
                        System.out.printf("sources count: %d%n", sources.size());
                        for (SeedInfo s : sources) {
                            System.out.printf("\taddress: %s, getPort: %d%n",
                                s.getInetAddress(),
                                s.getPort()
                            );
                        }
                        break;
                    }
                    case Command.DOWNLOAD: {
                        int fileId = scanner.nextInt();
                        if(client.downloadFile(fileId)) {
                            System.out.println("file with id " + fileId + " downloaded");
                        }
                        break;
                    }
                    case Command.STATS: {
                        List<LocalFileReference> files = client.getLocalFiles();
                        System.out.printf("local files count: %d%n", files.size());
                        for (LocalFileReference f : files) {
                            System.out.printf(
                                "\tfile: %s, id: %d, parts: %d/%d%n",
                                f.getName(),
                                f.getFileId(),
                                f.getReadyParts().size(),
                                f.getNumberOfParts()
                            );
                        }
                        break;
                    }
                    case Command.HELP : {
                        printUsage();
                        break;
                    }
                    default: {
                        System.out.printf("client: unknown command: %s%n", command);
                        break;
                    }
                }
            }
        } catch (TorrentException e) {
            System.out.println(e.getMassage());
            if (e.getException() != null) e.getException().printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static Short getPort(String[] args, Scanner scanner) {
        if (args.length != 0) {
            return Short.parseShort(args[0]);
        }
        return scanner.nextShort();
    }

    private static void printUsage() {
        String sep = System.lineSeparator();
        System.out.println("available commands:" + sep +
            Command.HELP + " - print this message" + sep +
            Command.LIST + " - list available files on the tracker" + sep +
            Command.SOURCES + " <id> - list seeds for file with the specified id" + sep +
            Command.UPLOAD + " <path> - add file to the tracker" + sep +
            Command.DOWNLOAD + " <id> - download file with the specified id" + sep +
            Command.EXIT + " - shutdown the client app" + sep
        );
    }

    private static class Command {
        static final String HELP = "help";
        static final String LIST = "list";
        static final String UPLOAD = "upload";
        static final String SOURCES = "sources";
        static final String EXIT = "exit";
        static final String DOWNLOAD = "download";
        static final String STATS = "stats";
    }
}
