package ru.hse.spb.protocol.deserialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.*;

import java.io.InputStream;

public interface ResponseDeserializer {
    static @NotNull ResponseDeserializer createDeserializer(InputStream input) {
        return new ResponseDeserializerImpl(DataDeserializer.createDeserializer(input));
    }

    @NotNull GetResponse parseGet() throws SerializationException;

    @NotNull ListResponse parseList() throws SerializationException;

    @NotNull SourcesResponse parseSources() throws SerializationException;

    @NotNull StatResponse parseStat() throws SerializationException;

    @NotNull UpdateResponse parseUpdate() throws SerializationException;

    @NotNull UploadResponse parseUpload() throws SerializationException;
}
