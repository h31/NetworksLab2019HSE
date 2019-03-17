package ru.ifmo.torrent.messages.client_tracker.response;

import ru.ifmo.torrent.messages.Response;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class UploadResponse extends Response {

    private int fileId;

    public UploadResponse() {
    }

    public UploadResponse(int fileId) {
        this.fileId = fileId;
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeInt(fileId);
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        fileId = in.readInt();
    }

    public int getFileId() {
        return fileId;
    }

}
