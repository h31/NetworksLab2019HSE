package ru.hse.spb.protocol.deserialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.ListResponse;

import java.io.InputStream;
import java.net.InetSocketAddress;
import java.util.List;

interface DataDeserializer {
    static @NotNull DataDeserializer createDeserializer(InputStream input) {
        return new DataDeserializerImpl(input);
    }

    boolean parseBoolean() throws SerializationException;

    byte parseByte() throws SerializationException;

    char parseChar() throws SerializationException;

    int parseInt() throws SerializationException;

    long parseLong() throws SerializationException;

    @NotNull String parseString() throws SerializationException;

    @NotNull InetSocketAddress parseAddress() throws SerializationException;

    @NotNull ListResponse.FileInfo parseFileInfo() throws SerializationException;

    @NotNull byte[] parseBytes(int size) throws SerializationException;

    @NotNull List<Integer> parseIntegers(int size) throws SerializationException;

    @NotNull List<InetSocketAddress> parseAddresses(int size) throws SerializationException;

    @NotNull List<ListResponse.FileInfo> parseFileInfos(int size) throws SerializationException;
}
