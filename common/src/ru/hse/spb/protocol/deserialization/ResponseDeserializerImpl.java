package ru.hse.spb.protocol.deserialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.Constants;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.*;

public class ResponseDeserializerImpl implements ResponseDeserializer {
    private final @NotNull DataDeserializer dataDeserializer;

    protected ResponseDeserializerImpl(@NotNull DataDeserializer dataDeserializer) {
        this.dataDeserializer = dataDeserializer;
    }

    @Override
    public @NotNull GetResponse parseGet() throws SerializationException {
        return new GetResponse(dataDeserializer.parseBytes(Constants.FILE_PART_SIZE));
    }

    @Override
    public @NotNull ListResponse parseList() throws SerializationException {
        return new ListResponse(dataDeserializer.parseFileInfos(dataDeserializer.parseInt()));
    }

    @Override
    public @NotNull SourcesResponse parseSources() throws SerializationException {
        return new SourcesResponse(dataDeserializer.parseAddresses(dataDeserializer.parseInt()));
    }

    @Override
    public @NotNull StatResponse parseStat() throws SerializationException {
        int count = dataDeserializer.parseInt();
        return new StatResponse(count, dataDeserializer.parseIntegers(count));
    }

    @Override
    public @NotNull UpdateResponse parseUpdate() throws SerializationException {
        return new UpdateResponse(dataDeserializer.parseBoolean());
    }

    @Override
    public @NotNull UploadResponse parseUpload() throws SerializationException {
        return new UploadResponse(dataDeserializer.parseInt());
    }
}
