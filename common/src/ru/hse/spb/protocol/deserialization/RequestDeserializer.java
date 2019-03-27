package ru.hse.spb.protocol.deserialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.request.AbstractRequest;

import java.io.InputStream;

public interface RequestDeserializer {
    enum Mode {
        CLIENT_SERVER, CLIENT_CLIENT
    }

    static @NotNull RequestDeserializer createClientServerDeserializer(@NotNull InputStream input) {
        return new RequestDeserializerImpl(DataDeserializer.createDeserializer(input), Mode.CLIENT_SERVER);
    }

    static @NotNull RequestDeserializer createClientClientDeserializer(@NotNull InputStream input) {
        return new RequestDeserializerImpl(DataDeserializer.createDeserializer(input), Mode.CLIENT_CLIENT);
    }

    @NotNull AbstractRequest parseRequest() throws SerializationException;
}
