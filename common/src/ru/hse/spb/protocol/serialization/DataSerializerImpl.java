package ru.hse.spb.protocol.serialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.ListResponse;

import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.util.Arrays;
import java.util.List;

public class DataSerializerImpl implements DataSerializer {
    private final @NotNull DataOutputStream output;

    protected DataSerializerImpl(@NotNull OutputStream output) {
        this.output = new DataOutputStream(output);
    }

    @Override
    public void writeBoolean(boolean value) throws SerializationException {
        try {
            output.writeBoolean(value);
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public void writeByte(byte value) throws SerializationException {
        try {
            output.writeByte(value);
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public void writeChar(char value) throws SerializationException {
        try {
            output.writeChar(value);
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public void writeInt(int value) throws SerializationException {
        try {
            output.writeInt(value);
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public void writeLong(long value) throws SerializationException {
        try {
            output.writeLong(value);
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public void writeString(@NotNull String value) throws SerializationException {
        try {
            output.writeUTF(value);
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public void writeAddress(@NotNull InetSocketAddress value) throws SerializationException {
        byte[] address = value.getAddress().getAddress();
        if (address.length != 4) {
            throw new SerializationException("Illegal ip address: " + Arrays.toString(address));
        }
        char port = (char) value.getPort();
        for (byte b : address) {
            writeByte(b);
        }
        writeChar(port);
    }

    @Override
    public void writeFileInfo(@NotNull ListResponse.FileInfo value) throws SerializationException {
        writeInt(value.getId());
        writeString(value.getName());
        writeLong(value.getSize());
    }

    @Override
    public void writeBytes(@NotNull byte[] value) throws SerializationException {
        for (byte b : value) {
            writeByte(b);
        }
    }

    @Override
    public void writeIntegers(@NotNull List<Integer> value) throws SerializationException {
        for (int integer : value) {
            writeInt(integer);
        }
    }

    @Override
    public void writeAddresses(@NotNull List<InetSocketAddress> value) throws SerializationException {
        for (InetSocketAddress inetSocketAddress : value) {
            writeAddress(inetSocketAddress);
        }
    }

    @Override
    public void writeFileInfos(@NotNull List<ListResponse.FileInfo> value) throws SerializationException {
        for (ListResponse.FileInfo fileInfo : value) {
            writeFileInfo(fileInfo);
        }
    }

    @Override
    public void flush() throws SerializationException {
        try {
            output.flush();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }
}
