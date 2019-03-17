package ru.ifmo.torrent.tracker;

import ru.ifmo.torrent.util.Config;

import java.nio.file.Files;
import java.nio.file.Path;

public class TrackerConfig extends Config {

    public static final int TIMEOUT = 5 * 1000;
    public static final short TRACKER_PORT = 8081;
    public static final int THREADS_COUNT = 8;
    public static final int UPDATE_RATE_SEC = 180;

    public static final String TRACKER_STATE_FILE = "tracker_state_file";

    private TrackerConfig() {
    }

    public static Path getMetaDir() {
        Path storage = TRACKER_STORAGE;
        if (Files.notExists(storage)) {
            storage.toFile().mkdirs();
        }
        return storage;
    }
}
