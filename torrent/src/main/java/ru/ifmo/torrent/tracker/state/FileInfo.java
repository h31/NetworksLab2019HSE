package ru.ifmo.torrent.tracker.state;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.Objects;

public class FileInfo {

    private int id;
    private String name;
    private long size;

    public FileInfo(int id, String name, long size) {
        this.id = id;
        this.name = name;
        this.size = size;
    }

    public int getId() {
        return id;
    }

    public long getSize() {
        return size;
    }

    public String getName() {
        return name;
    }

    public static FileInfo readFrom(DataInputStream in) throws IOException {
        int id = in.readInt();
        String name = in.readUTF();
        long size = in.readLong();
        return new FileInfo(id, name, size);
    }

    public void write(DataOutputStream out) throws IOException {
        out.writeInt(id);
        out.writeUTF(name);
        out.writeLong(size);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        FileInfo fileInfo = (FileInfo) o;
        return id == fileInfo.id &&
            size == fileInfo.size &&
            Objects.equals(name, fileInfo.name);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id, name, size);
    }
}
