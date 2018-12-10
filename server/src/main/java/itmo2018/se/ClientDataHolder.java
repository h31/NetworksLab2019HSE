package itmo2018.se;

import com.sun.xml.internal.messaging.saaj.util.ByteInputStream;

import java.io.*;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class ClientDataHolder {
    private byte[] bytes;
    private List<Byte> bytesOfZize = new ArrayList<>(4);
    private int packageSize = -1;
    private int currentSize = 0;

    public void read(ByteBuffer buffer) {
        if (packageSize == -1) {
            int intSize = 4;
            while (buffer.position() < buffer.limit() && bytesOfZize.size() < intSize) {
                bytesOfZize.add(buffer.get());
            }
            if (bytesOfZize.size() == intSize) {
                packageSize = bytesToInt(bytesOfZize);
                bytes = new byte[packageSize];
            }
        }
        while (buffer.position() < buffer.limit() && currentSize < packageSize) {
            bytes[currentSize] = buffer.get();
            currentSize++;
        }
    }

    public boolean isReady() {
        return packageSize == currentSize;
    }

    public InputStream getContent() {
        return new ByteArrayInputStream(bytes);
    }

    public void reset() {
        packageSize = -1;
        currentSize = 0;
        bytesOfZize = new ArrayList<>(4);
    }

    private int bytesToInt(List<Byte> b) {
        if (b.size() != 4) {
            return 0;
        }
        return b.get(0) << 24 | (b.get(1) & 0xff) << 16 | (b.get(2) & 0xff) << 8 | (b.get(3) & 0xff);
    }
}
