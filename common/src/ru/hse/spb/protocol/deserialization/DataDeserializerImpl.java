package ru.hse.spb.protocol.deserialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.ListResponse;

import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.List;

public class DataDeserializerImpl implements DataDeserializer {
    private final @NotNull DataInputStream input;

    protected DataDeserializerImpl(@NotNull InputStream input) {
        this.input = new DataInputStream(input);
    }

    @Override
    public boolean parseBoolean() throws SerializationException {
        try {
            return input.readBoolean();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public byte parseByte() throws SerializationException {
        try {
            return input.readByte();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public char parseChar() throws SerializationException {
        try {
            return input.readChar();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public int parseInt() throws SerializationException {
        try {
            return input.readInt();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public long parseLong() throws SerializationException {
        try {
            return input.readLong();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public @NotNull String parseString() throws SerializationException {
        try {
            return input.readUTF();
        } catch (IOException e) {
            throw new SerializationException(e);
        }
    }

    @Override
    public @NotNull InetSocketAddress parseAddress() throws SerializationException {
        byte[] bytes = new byte[4];
        for (int i = 0; i < 4; i++) {
            bytes[i] = parseByte();
        }
        InetAddress inetAddress;
        try {
            inetAddress = Inet4Address.getByAddress(bytes);
        } catch (UnknownHostException e) {
            throw new SerializationException(e);
        }
        int port = (int) parseChar();
        return new InetSocketAddress(inetAddress, port);
    }

    @Override
    public @NotNull ListResponse.FileInfo parseFileInfo() throws SerializationException {
        return new ListResponse.FileInfo(parseInt(), parseString(), parseLong());
    }

    @Override
    public @NotNull byte[] parseBytes(int size) throws SerializationException {
        byte[] result = new byte[size];
        for (int i = 0; i < size; i++) {
            result[i] = parseByte();
        }
        return result;
    }

    @Override
    public @NotNull List<Integer> parseIntegers(int size) throws SerializationException {
        List<Integer> result = new ArrayList<>();
        for (int i = 0; i < size; i++) {
            result.add(parseInt());
        }
        return result;
    }

    @Override
    public @NotNull List<InetSocketAddress> parseAddresses(int size) throws SerializationException {
        List<InetSocketAddress> result = new ArrayList<>();
        for (int i = 0; i < size; i++) {
            result.add(parseAddress());
        }
        return result;
    }

    @Override
    public @NotNull List<ListResponse.FileInfo> parseFileInfos(int size) throws SerializationException {
        List<ListResponse.FileInfo> result = new ArrayList<>();
        for (int i = 0; i < size; i++) {
            result.add(parseFileInfo());
        }
        return result;
    }
}
