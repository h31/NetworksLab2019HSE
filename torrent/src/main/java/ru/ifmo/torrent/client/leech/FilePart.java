package ru.ifmo.torrent.client.leech;

import java.util.Objects;

public class FilePart {
    private final int fileId;
    private final int num;

    public FilePart(int fileId, int partNum) {
        this.fileId = fileId;
        this.num = partNum;
    }

    public int getFileId() {
        return fileId;
    }

    public int getPartNum() {
        return num;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        FilePart filePart = (FilePart) o;
        return fileId == filePart.fileId && num == filePart.num;
    }

    @Override
    public int hashCode() {
        return Objects.hash(fileId, num);
    }

}
