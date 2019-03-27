package ru.hse.spb.protocol.serialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.request.*;

import java.io.OutputStream;

public interface RequestSerializer {
    static @NotNull RequestSerializer createSerializer(OutputStream output) {
        return new RequestSerializerImpl(DataSerializer.createSerializer(output));
    }

    void write(GetRequest request) throws SerializationException;

    void write(ListRequest request) throws SerializationException;

    void write(SourcesRequest request) throws SerializationException;

    void write(StatRequest request) throws SerializationException;

    void write(UpdateRequest request) throws SerializationException;

    void write(UploadRequest request) throws SerializationException;
}
