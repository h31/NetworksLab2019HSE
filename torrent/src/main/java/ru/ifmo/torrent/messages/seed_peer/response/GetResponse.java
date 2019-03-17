package ru.ifmo.torrent.messages.seed_peer.response;

import org.apache.commons.io.IOUtils;
import ru.ifmo.torrent.client.ClientConfig;
import ru.ifmo.torrent.messages.Response;

import java.io.*;

public class GetResponse extends Response {

    private byte[] content;
    private int size;

    public GetResponse() {}

    public GetResponse(InputStream in, int size) throws IOException {
        this.size = size;
        this.content = new byte[size];
        int totalReaded = 0;
        while(totalReaded != size) {
            totalReaded += in.read(content);
        }
    }

    @Override
    public void write(DataOutputStream out) throws IOException {
        out.write(content);
    }

    @Override
    public void read(DataInputStream in) throws IOException {
        ByteArrayOutputStream buffer = new ByteArrayOutputStream(size);
        IOUtils.copy(in, buffer);
        content = buffer.toByteArray();
    }

    public byte[] getContent() {
        return content;
    }
}
