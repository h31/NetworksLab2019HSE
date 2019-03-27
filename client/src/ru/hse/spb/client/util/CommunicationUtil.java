package ru.hse.spb.client.util;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.deserialization.ResponseDeserializer;
import ru.hse.spb.protocol.request.*;
import ru.hse.spb.protocol.response.*;
import ru.hse.spb.protocol.serialization.RequestSerializer;

import java.io.IOException;
import java.net.Socket;

public class CommunicationUtil {
    private CommunicationUtil() {
    }

    private static ResponseDeserializer createResponseDeserializer(@NotNull Socket socket) throws IOException {
        return ResponseDeserializer.createDeserializer(socket.getInputStream());
    }

    private static RequestSerializer createRequestSerializer(@NotNull Socket socket) throws IOException {
        return RequestSerializer.createSerializer(socket.getOutputStream());
    }

    public static UpdateResponse sendRequest(@NotNull Socket socket, @NotNull UpdateRequest request) throws IOException {
        createRequestSerializer(socket).write(request);
        return createResponseDeserializer(socket).parseUpdate();
    }

    public static SourcesResponse sendRequest(@NotNull Socket socket, @NotNull SourcesRequest request) throws IOException {
        createRequestSerializer(socket).write(request);
        return createResponseDeserializer(socket).parseSources();
    }

    public static UploadResponse sendRequest(@NotNull Socket socket, @NotNull UploadRequest request) throws IOException {
        createRequestSerializer(socket).write(request);
        return createResponseDeserializer(socket).parseUpload();
    }

    public static ListResponse sendRequest(@NotNull Socket socket, @NotNull ListRequest request) throws IOException {
        createRequestSerializer(socket).write(request);
        return createResponseDeserializer(socket).parseList();
    }

    public static GetResponse sendRequest(@NotNull Socket socket, @NotNull GetRequest request) throws IOException {
        createRequestSerializer(socket).write(request);
        return createResponseDeserializer(socket).parseGet();
    }

    public static StatResponse sendRequest(@NotNull Socket socket, @NotNull StatRequest request) throws IOException {
        createRequestSerializer(socket).write(request);
        return createResponseDeserializer(socket).parseStat();
    }


}
