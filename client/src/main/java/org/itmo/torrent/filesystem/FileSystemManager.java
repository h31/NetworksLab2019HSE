package org.itmo.torrent.filesystem;

import org.jetbrains.annotations.NotNull;

import java.nio.file.Path;
import java.nio.file.Paths;

public class FileSystemManager {
    @NotNull
    private static final String DOWNLOADS_FOLDER_NAME = "downloads";

    @NotNull
    private Path rootFolderPath = Paths.get(".");

    private FileSystemManager() {
    }

    @NotNull
    public Path getRootFolderPath() {
        return rootFolderPath;
    }

    public void setRootFolderPath(@NotNull final Path rootDirPath) {
        this.rootFolderPath = rootDirPath;
    }

    @NotNull
    public Path getDownloadsFolderPath() {
        return rootFolderPath.resolve(DOWNLOADS_FOLDER_NAME);
    }

    @NotNull
    public static FileSystemManager getInstance()  {
        return SingletonHolder.instance;
    }

    private static class SingletonHolder {
        @NotNull
        private static final FileSystemManager instance = new FileSystemManager();
    }
}
