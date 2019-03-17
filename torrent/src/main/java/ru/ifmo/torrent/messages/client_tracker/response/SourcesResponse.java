package ru.ifmo.torrent.messages.client_tracker.response;

import ru.ifmo.torrent.messages.Response;
import ru.ifmo.torrent.tracker.state.SeedInfo;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class SourcesResponse extends Response {
    private int fileId;
    private List<SeedInfo> clients;

    public SourcesResponse() {
        clients = new ArrayList<>();
    }

    public SourcesResponse(int fileId, List<SeedInfo> clients) {
        this.fileId = fileId;
        this.clients = clients;
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeInt(clients.size());
        for (SeedInfo c : clients) {
            out.write(c.getIP());
            out.writeShort(c.getPort());
        }
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        int count = in.readInt();
        for (int i = 0; i < count; i++) {
            byte[] IP = new byte[4];
            in.readFully(IP);
            short port = in.readShort();
            clients.add(new SeedInfo(port, IP));
        }
    }

    public int getFileId() {
        return fileId;
    }

    public List<SeedInfo> getClients() {
        return clients;
    }

    public void setFileId(int fileId) {
        this.fileId = fileId;
    }

}
