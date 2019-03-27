package org.itmo.torrent.filesystem;

import org.jetbrains.annotations.NotNull;

import java.io.File;
import java.io.IOException;
import java.util.BitSet;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class FileSeedInfo extends FileInfo {
    @NotNull
    private final File file;
    @NotNull
    private final BitSet availableChunks;

    public FileSeedInfo(final int id, @NotNull final String name, final long size,
                        @NotNull final File file, @NotNull final BitSet availableChunks) {
        super(id, name, size);
        this.file = file;
        this.availableChunks = availableChunks;
    }

    public FileSeedInfo(final int id, @NotNull final String name, final long size,
                        @NotNull final File file) {
        super(id, name, size);
        this.file = file;
        final int chunksNumber = (int) getChunksNumber();
        availableChunks = new BitSet(chunksNumber);
        availableChunks.set(0, chunksNumber);
    }

    public FileSeedInfo(@NotNull final FileInfo fileInfo) throws IOException {
        super(fileInfo.getId(), fileInfo.getName(), fileInfo.getSize());
        this.file = createFile(fileInfo);
        final int chunksNumber = (int) getChunksNumber();
        availableChunks = new BitSet(chunksNumber);
    }

    @NotNull
    public File getFile() {
        return file;
    }

    @NotNull
    public BitSet getAvailableChunks() {
        return availableChunks;
    }

    @NotNull
    public Set<Integer> getSetAvailableChunks() {
        return IntStream.range(0, availableChunks.length())
                .filter(this::isChunkAvailable)
                .boxed()
                .collect(Collectors.toSet());
    }

    public boolean isChunkAvailable(final int chunkId) {
        return availableChunks.get(chunkId);
    }

    public void setChunkAvailable(final int chunkId) {
        availableChunks.set(chunkId);
    }

    public long getChunksNumber() {
        return (getSize() + FILE_CHUNK_SIZE - 1) / FILE_CHUNK_SIZE;
    }

    public long getChunkSize(final int chunkId) {
        if (chunkId < getChunksNumber() - 1 || getSize() % FILE_CHUNK_SIZE == 0) {
            return FILE_CHUNK_SIZE;
        } else {
            return getSize() % FILE_CHUNK_SIZE;
        }
    }

    @NotNull
    private File createFile(@NotNull FileInfo fileInfo) throws IOException {
        final FileSystemManager manager = FileSystemManager.getInstance();
        final File downloadsFolder = manager.getDownloadsFolderPath().toFile();

        if (!downloadsFolder.exists() && !downloadsFolder.mkdirs()) {
            throw new IllegalArgumentException(
                    String.format("Cannot create directory %s", downloadsFolder.getPath()));
        }

        if (!downloadsFolder.isDirectory()) {
            throw new IllegalArgumentException(
                    String.format("%s is not a directory", downloadsFolder.getPath()));
        }

        final File file = manager.getDownloadsFolderPath().resolve(fileInfo.getName()).toFile();
        if (!file.exists() && !file.createNewFile()) {
            throw new IllegalArgumentException(
                    String.format("Cannot create file %s", file.getPath()));
        }
        return file;
    }
}
