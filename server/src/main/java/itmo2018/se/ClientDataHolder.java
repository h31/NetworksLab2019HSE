package itmo2018.se;

import java.io.*;
import java.nio.ByteBuffer;

public class ClientDataHolder {
    private byte[] bytes;
    private int packageSize = -1;
    private int currentSize = 0;

    public void read(ByteBuffer buffer) {
        int intSize = 4;
        if (packageSize == -1 && buffer.limit() - buffer.position() >= intSize) {
            packageSize = buffer.getInt();
            bytes = new byte[packageSize];
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
    }
}
