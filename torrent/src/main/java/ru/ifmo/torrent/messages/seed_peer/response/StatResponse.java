package ru.ifmo.torrent.messages.seed_peer.response;

import ru.ifmo.torrent.messages.Response;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class StatResponse extends Response {

    private List<Integer> availableParts;

    public StatResponse() {}

    public StatResponse(List<Integer> availableParts) {
        this.availableParts = availableParts;
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.writeInt(availableParts.size());
        for (Integer i: availableParts) {
            out.writeInt(i);
        }
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        availableParts = new ArrayList<>();
        int count = in.readInt();
        for (int i = 0; i < count; i++) {
            availableParts.add(in.readInt());
        }
    }

    public List<Integer> getAvailableParts() {
        return availableParts;
    }
}
