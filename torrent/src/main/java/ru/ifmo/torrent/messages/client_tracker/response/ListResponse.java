package ru.ifmo.torrent.messages.client_tracker.response;

import ru.ifmo.torrent.messages.Response;
import ru.ifmo.torrent.tracker.state.FileInfo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class ListResponse extends Response {
    private List<FileInfo> files;

    public ListResponse() {
        files = new ArrayList<>();
    }

    public ListResponse(List<FileInfo> files) {
        this.files = files;
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeInt(files.size());
        for (FileInfo f : files) {
            out.writeInt(f.getId());
            out.writeUTF(f.getName());
            out.writeLong(f.getSize());
        }
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        int count = in.readInt();
        for (int i = 0; i < count; i++) {
            int id = in.readInt();
            String name = in.readUTF();
            long size = in.readLong();
            FileInfo f = new FileInfo(id, name, size);
            files.add(f);
        }

    }

    public List<FileInfo> getFiles() {
        return files;
    }
}
