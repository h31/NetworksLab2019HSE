package ru.ifmo.torrent.messages.client_tracker.requests;

import ru.ifmo.torrent.messages.Request;
import ru.ifmo.torrent.messages.client_tracker.Marker;
import ru.ifmo.torrent.messages.client_tracker.response.UploadResponse;
import ru.ifmo.torrent.messages.Response;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public class UploadRequest extends Request<UploadResponse> {

    private String fileName;
    private long fileSize;

    public UploadRequest() {}

    public UploadRequest(Path file) throws IOException {
        this.fileName = file.getFileName().toString();
        this.fileSize = Files.size(file);
    }

    @Override
    public byte marker() {
        return Marker.UPLOAD;
    }

    @Override
    public UploadResponse getEmptyResponse() {
        return new UploadResponse();
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeByte(marker());
        out.writeUTF(fileName);
        out.writeLong(fileSize);
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        fileName = in.readUTF();
        fileSize = in.readLong();
    }

    public String getFileName() {
        return fileName;
    }

    public long getFileSize() {
        return fileSize;
    }

}
