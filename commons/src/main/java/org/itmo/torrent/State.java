package org.itmo.torrent;

import org.jetbrains.annotations.NotNull;

import java.io.*;
import java.nio.file.Path;

public interface State {

    default void save(@NotNull final Path rootFolderPath) throws IOException {
        final File rootFolder = rootFolderPath.toFile();

        if (!rootFolder.exists() && !rootFolder.mkdirs()) {
            throw new IllegalArgumentException(
                    String.format("Cannot create directory %s", rootFolderPath));
        }

        if (!rootFolder.isDirectory()) {
            throw new IllegalArgumentException(
                    String.format("%s is not a directory", rootFolderPath));
        }

        final Path stateFilePath = rootFolderPath.resolve(getStateFileName());
        final File stateFile = stateFilePath.toFile();

        if (!stateFile.exists() && !stateFile.createNewFile()) {
            throw new IllegalArgumentException(
                    String.format("Cannot create state file %s", stateFilePath));
        }

        try (OutputStream out = new FileOutputStream(stateFile)) {
            toOutputStream(out);
        }
    }

    default void restore(@NotNull final Path rootFolderPath)
            throws IOException, ClassNotFoundException {
        final Path stateFilePath = rootFolderPath.resolve(getStateFileName());
        final File stateFile = stateFilePath.toFile();

        if (!stateFile.exists()) {
            return;
        }

        if (!stateFile.isFile()) {
            throw new IllegalArgumentException(
                    String.format("%s is not a regular file", stateFilePath));
        }

        try (InputStream in = new FileInputStream(stateFile)) {
            fromInputStream(in);
        }
    }

    @NotNull
    String getStateFileName();

    void toOutputStream(@NotNull final OutputStream out) throws IOException;

    void fromInputStream(@NotNull final InputStream in) throws IOException, ClassNotFoundException;
}
