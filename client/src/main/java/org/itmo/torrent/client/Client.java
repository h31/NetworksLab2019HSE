package org.itmo.torrent.client;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.network.Address;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Set;

public interface Client {

    @NotNull
    List<FileInfo> executeList() throws IOException;

    int executeUpload(@NotNull final File file) throws IOException;

    @NotNull
    Set<Address> executeSources(final int fileId) throws IOException;

    boolean executeUpdate() throws IOException;

    void executeDownload(final int fileId) throws IOException;

    @NotNull
    Set<Integer> executeStat(@NotNull final Address address,
                             final int fileId) throws IOException;

    @Nullable
    InputStream executeGet(@NotNull final Address address,
                           final int fileId, final int fileChunkId) throws IOException;
}
