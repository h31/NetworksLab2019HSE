package ru.ifmo.torrent.messages.seed_peer.requests;

import ru.ifmo.torrent.messages.seed_peer.Marker;
import ru.ifmo.torrent.messages.seed_peer.response.StatResponse;
import ru.ifmo.torrent.messages.Request;
import ru.ifmo.torrent.messages.Response;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;

public class StatRequest extends Request<StatResponse> {
    private int fileId;

    public StatRequest() {
    }

    public StatRequest(int fileId) {
        this.fileId = fileId;
    }

    @Override
    public byte marker() {
        return Marker.STAT;
    }

    @Override
    public StatResponse getEmptyResponse() {
        return new StatResponse();
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
