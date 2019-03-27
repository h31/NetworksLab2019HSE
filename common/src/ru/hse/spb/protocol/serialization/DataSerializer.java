package ru.hse.spb.protocol.serialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.ListResponse;

import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.util.List;

public interface DataSerializer {
    static @NotNull DataSerializer createSerializer(OutputStream output) {
        return new DataSerializerImpl(output);
    }

    void writeBoolean(boolean value) throws SerializationException;

    void writeByte(byte value) throws SerializationException;

    void writeChar(char value) throws SerializationException;

    void writeInt(int value) throws SerializationException;

    void writeLong(long value) throws SerializationException;

    void writeString(@NotNull String value) throws SerializationException;

    void writeAddress(@NotNull InetSocketAddress value) throws SerializationException;

    void writeFileInfo(@NotNull ListResponse.FileInfo value) throws SerializationException;

    void writeBytes(@NotNull byte[] value) throws SerializationException;

    void writeIntegers(@NotNull List<Integer> value) throws SerializationException;

    void writeAddresses(@NotNull List<InetSocketAddress> value) throws SerializationException;

    void writeFileInfos(@NotNull List<ListResponse.FileInfo> value) throws SerializationException;

    void flush() throws SerializationException;
}
