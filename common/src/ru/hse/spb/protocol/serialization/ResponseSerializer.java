package ru.hse.spb.protocol.serialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.*;

import java.io.OutputStream;

public interface ResponseSerializer {
    static @NotNull ResponseSerializer createSerializer(OutputStream output) {
        return new ResponseSerializerImpl(DataSerializer.createSerializer(output));
    }

    void write(GetResponse response) throws SerializationException;

    void write(ListResponse response) throws SerializationException;

    void write(SourcesResponse response) throws SerializationException;

    void write(StatResponse response) throws SerializationException;

    void write(UpdateResponse response) throws SerializationException;

    void write(UploadResponse response) throws SerializationException;
}
