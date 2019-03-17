package ru.ifmo.torrent.messages.client_tracker.requests;

import ru.ifmo.torrent.messages.Request;
import ru.ifmo.torrent.messages.client_tracker.Marker;
import ru.ifmo.torrent.messages.client_tracker.response.SourcesResponse;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class SourcesRequest extends Request<SourcesResponse> {

    private int fileId;

    public SourcesRequest() {}

    public SourcesRequest(int fileId) {
        this.fileId = fileId;
    }

    @Override
    public byte marker() {
        return Marker.SOURCES;
    }

    @Override
    public SourcesResponse getEmptyResponse() {
        return new SourcesResponse();
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeByte(marker());
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
