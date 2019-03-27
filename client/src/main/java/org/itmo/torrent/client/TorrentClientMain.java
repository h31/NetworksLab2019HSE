package org.itmo.torrent.client;

import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.network.Address;

import java.io.File;
import java.io.IOException;
import java.nio.file.Paths;
import java.util.List;
import java.util.Scanner;
import java.util.Set;

public class TorrentClientMain {
    @NotNull
    private static final String USAGE = "client <client-port>";

    @NotNull
    private static final String LIST_CMD = "list";
    @NotNull
    private static final String UPLOAD_CMD = "upload";
    @NotNull
    private static final String SOURCES_CMD = "sources";
    @NotNull
    private static final String UPDATE_CMD = "update";
    @NotNull
    private static final String DOWNLOAD_CMD = "download";
    @NotNull
    private static final String EXIT_CMD = "exit";

    @NotNull
    private static final String COMMANDS = String.format(
                    "- %s\n" +
                    "- %s <file-path>\n" +
                    "- %s <file-id>\n" +
                    "- %s\n" +
                    "- %s <file-id>\n" +
                    "- %s",
            LIST_CMD, UPLOAD_CMD, SOURCES_CMD, UPDATE_CMD, DOWNLOAD_CMD, EXIT_CMD
    );

    private TorrentClientMain() {
    }

    public static void main(String[] args) {
        if (args.length < 1) {
            System.err.println(USAGE);
            return;
        }

        final short port = Short.parseShort(args[0]);
        final Scanner scanner = new Scanner(System.in);
        try (TorrentClient client = new TorrentClient(port, Paths.get("."))) {
            System.out.println("Hello!");
            while (true) {
                try {
                    final String command = scanner.next();
                    switch (command) {
                        case LIST_CMD: {
                            final List<FileInfo> fileInfos = client.executeList();
                            System.out.println("Files on tracker: " + fileInfos.size());
                            fileInfos.forEach(System.out::println);
                            break;
                        }
                        case UPLOAD_CMD: {
                            final File file = Paths.get(scanner.next()).toFile();
                            final int fileId = client.executeUpload(file);
                            System.out.println("Success! New file ID: " + fileId);
                            break;
                        }
                        case SOURCES_CMD: {
                            final int fileId = scanner.nextInt();
                            final Set<Address> addresses = client.executeSources(fileId);
                            System.out.println("Users who have this file: " + addresses.size());
                            addresses.forEach(System.out::println);
                            break;
                        }
                        case UPDATE_CMD: {
                            final boolean result = client.executeUpdate();
                            System.out.println(result ? "Success!" : "Fail?");
                            break;
                        }
                        case DOWNLOAD_CMD: {
                            final int fileId = scanner.nextInt();
                            client.executeDownload(fileId);
                            break;
                        }
                        case EXIT_CMD: {
                            System.out.println("Bye-bye!");
                            System.exit(0);  // wtf
                        }
                        default: {
                            System.err.println("Unknown command: " + command);
                            System.out.println(COMMANDS);
                        }
                    }
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                }
            }
        } catch (IOException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }
}
