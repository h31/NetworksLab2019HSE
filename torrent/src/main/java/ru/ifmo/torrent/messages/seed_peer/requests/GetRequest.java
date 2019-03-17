package ru.ifmo.torrent.messages.seed_peer.requests;

import ru.ifmo.torrent.messages.seed_peer.Marker;
import ru.ifmo.torrent.messages.seed_peer.response.GetResponse;
import ru.ifmo.torrent.messages.Request;
import ru.ifmo.torrent.messages.Response;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class GetRequest extends Request<GetResponse> {

    private int fileId;
    private int part;

    public GetRequest() {
    }

    public GetRequest(int fileId, int part) {
        this.fileId = fileId;
        this.part = part;
    }

    @Override
    public byte marker() {
        return Marker.GET;
    }

    @Override
    public GetResponse getEmptyResponse() {
        return new GetResponse();
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeByte(marker());
        out.writeInt(fileId);
        out.writeInt(part);
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        fileId = in.readInt();
        part = in.readInt();
    }

    public int getFileId() {
        return fileId;
    }

    public int getPart() {
        return part;
    }

}
