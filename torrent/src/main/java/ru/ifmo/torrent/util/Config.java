package ru.ifmo.torrent.util;

import java.nio.file.Path;
import java.nio.file.Paths;

public abstract class Config {

    public static Path CWD = Paths.get(System.getProperty("user.dir")).normalize();

    public static final Path TORRENT_DIR = CWD.resolve("torrent");
    protected static final Path TORRENT_META_INFO_DIR = TORRENT_DIR.resolve(".metainfo");
    protected static final Path TRACKER_STORAGE = TORRENT_META_INFO_DIR.resolve("tracker");
    protected static final Path CLIENT_STORAGE = TORRENT_META_INFO_DIR.resolve("client");
}
