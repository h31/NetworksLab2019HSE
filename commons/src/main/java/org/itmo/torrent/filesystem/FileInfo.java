package org.itmo.torrent.filesystem;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.Objects;

public class FileInfo {
    public final static long FILE_CHUNK_SIZE = 10 * 1024;

    private final int id;

    @NotNull
    private final String name;

    private final long size;

    public FileInfo(final int id, @NotNull final String name, final long size) {
        this.id = id;
        this.name = name;
        this.size = size;
    }

    public int getId() {
        return id;
    }

    @NotNull
    public String getName() {
        return name;
    }

    public long getSize() {
        return size;
    }

    @Override
    public String toString() {
        return String.format("FileInfo{id=%d, name=%s, size=%d}", id, name, size);
    }

    @Override
    public boolean equals(@Nullable final Object object) {
        if (this == object) {
            return true;
        }

        if (object == null || getClass() != object.getClass()) {
            return false;
        }

        final FileInfo fileInfo = (FileInfo) object;
        return getId() == fileInfo.getId() &&
                getSize() == fileInfo.getSize() &&
                Objects.equals(getName(), fileInfo.getName());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getId(), getName(), getSize());
    }
}
