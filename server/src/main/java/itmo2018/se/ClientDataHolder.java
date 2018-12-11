package itmo2018.se;

import java.io.*;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

public class ClientDataHolder {
    private byte[] inputContent;
    private Queue<ByteBuffer> outputContentQueue = new LinkedList<>();
    private ClientInfo info;
    private List<Byte> bytesOfZize = new ArrayList<>(4);
    private int packageSize = -1;
    private int currentSize = 0;

    public ClientDataHolder(ClientInfo info) {
        this.info = info;
    }

    public void read(ByteBuffer buffer) {
        if (packageSize == -1) {
            int intSize = 4;
            while (buffer.position() < buffer.limit() && bytesOfZize.size() < intSize) {
                bytesOfZize.add(buffer.get());
            }
            if (bytesOfZize.size() == intSize) {
                packageSize = bytesToInt(bytesOfZize);
                inputContent = new byte[packageSize];
            }
        }
        while (buffer.position() < buffer.limit() && currentSize < packageSize) {
            inputContent[currentSize] = buffer.get();
            currentSize++;
        }
    }

    public boolean isReady() {
        return packageSize == currentSize;
    }

    public InputStream getContent() {
        return new ByteArrayInputStream(inputContent);
    }

    public void reset() {
        packageSize = -1;
        currentSize = 0;
        bytesOfZize = new ArrayList<>(4);
    }

    public ClientInfo getInfo() {
        return info;
    }

    private int bytesToInt(List<Byte> b) {
        if (b.size() != 4) {
            return 0;
        }
        return b.get(0) << 24 | (b.get(1) & 0xff) << 16 | (b.get(2) & 0xff) << 8 | (b.get(3) & 0xff);
    }
}
