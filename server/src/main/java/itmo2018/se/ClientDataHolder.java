package itmo2018.se;

import java.io.*;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ClientDataHolder {
    private byte[] request;
    private Queue<ByteBuffer> responseQueue = new ConcurrentLinkedQueue<>();
    private ClientInfo clientInfo;

    private List<Byte> bytesOfZize = new ArrayList<>(4);
    private int packageSize = -1;
    private int currentSize = 0;

    public ClientDataHolder(ClientInfo clientInfo) {
        this.clientInfo = clientInfo;
    }

    public void read(ByteBuffer buffer) {
        if (packageSize == -1) {
            int intSize = 4;
            while (buffer.position() < buffer.limit() && bytesOfZize.size() < intSize) {
                bytesOfZize.add(buffer.get());
            }
            if (bytesOfZize.size() == intSize) {
                packageSize = bytesToInt(bytesOfZize);
                request = new byte[packageSize];
            }
        }
        while (buffer.position() < buffer.limit() && currentSize < packageSize) {
            request[currentSize] = buffer.get();
            currentSize++;
        }
    }

    public boolean requestIsReady() {
        return packageSize == currentSize;
    }

    public byte[] getRequest() {
        return request;
    }

    public void resetRequest() {
        packageSize = -1;
        currentSize = 0;
        bytesOfZize = new ArrayList<>(4);
    }

    public ClientInfo getClientInfo() {
        return clientInfo;
    }

    public void addResponse(ByteBuffer response) {
        responseQueue.add(response);
    }

    public ByteBuffer getResponse() {
        return responseQueue.poll();
    }

    private int bytesToInt(List<Byte> b) {
        if (b.size() != 4) {
            return 0;
        }
        return b.get(0) << 24 | (b.get(1) & 0xff) << 16 | (b.get(2) & 0xff) << 8 | (b.get(3) & 0xff);
    }
}
