package ru.ifmo.torrent.client;

import ru.ifmo.torrent.util.Config;

import java.nio.file.Files;
import java.nio.file.Path;

public class ClientConfig extends Config {

    public static final int FILE_PART_SIZE = 1024 * 1024 * 10;
    public static final int UPDATE_RATE_SEC = 7;
    public static final int DOWNLOAD_RATE_SEC = 2;
    public static final int TRACKER_PORT = 8081;
    public static final int SEED_THREADS_COUNT = 4;
    public static final int DOWNLOADS_LIMIT = 5;

    public static final String PARTS_STORAGE = "parts";
    public static final String LOCAL_FILES_FILE = "local_files_manager_file";

    private ClientConfig() {}

    public static Path getMetaDir() {
        Path storage = CLIENT_STORAGE;
        if(Files.notExists(storage)) {
            storage.toFile().mkdirs();
        }
        return storage;
    }

}
