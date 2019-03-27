package ru.hse.spb.protocol.deserialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.request.*;

public class RequestDeserializerImpl implements RequestDeserializer {
    private final @NotNull DataDeserializer dataDeserializer;
    private final Mode mode;

    protected RequestDeserializerImpl(@NotNull DataDeserializer dataDeserializer, Mode mode) {
        this.dataDeserializer = dataDeserializer;
        this.mode = mode;
    }

    @Override
    public @NotNull AbstractRequest parseRequest() throws SerializationException {
        byte type = dataDeserializer.parseByte();
        switch (mode) {
            case CLIENT_CLIENT:
                return parseClientClientRequest(type);
            case CLIENT_SERVER:
                return parseClientServerRequest(type);
            default:
                throw new IllegalStateException();
        }
    }

    private @NotNull AbstractRequest parseClientClientRequest(byte type) throws SerializationException {
        switch (type) {
            case StatRequest.TYPE:
                return parseStatRequest();
            case GetRequest.TYPE:
                return parseGetRequest();
            default:
                throw new SerializationException("Unknown request type: " + type);
        }
    }

    private @NotNull AbstractRequest parseClientServerRequest(byte type) throws SerializationException {
        switch (type) {
            case ListRequest.TYPE:
                return parseListRequest();
            case UploadRequest.TYPE:
                return parseUploadRequest();
            case SourcesRequest.TYPE:
                return parseSourcesRequest();
            case UpdateRequest.TYPE:
                return parseUpdateRequest();
            default:
                throw new SerializationException("Unknown request type: " + type);
        }
    }

    private @NotNull GetRequest parseGetRequest() throws SerializationException {
        return new GetRequest(dataDeserializer.parseInt(), dataDeserializer.parseInt());
    }

    private @NotNull ListRequest parseListRequest() {
        return new ListRequest();
    }

    private @NotNull SourcesRequest parseSourcesRequest() throws SerializationException {
        return new SourcesRequest(dataDeserializer.parseInt());
    }

    private @NotNull StatRequest parseStatRequest() throws SerializationException {
        return new StatRequest(dataDeserializer.parseInt());
    }

    private @NotNull UpdateRequest parseUpdateRequest() throws SerializationException {
        return new UpdateRequest(dataDeserializer.parseChar(), dataDeserializer.parseIntegers(dataDeserializer.parseInt()));
    }

    private @NotNull UploadRequest parseUploadRequest() throws SerializationException {
        return new UploadRequest(dataDeserializer.parseString(), dataDeserializer.parseLong());
    }
}