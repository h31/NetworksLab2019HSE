package ru.hse.spb.protocol.serialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.response.*;

public class ResponseSerializerImpl implements ResponseSerializer {
    private final @NotNull DataSerializer dataSerializer;

    protected ResponseSerializerImpl(@NotNull DataSerializer dataSerializer) {
        this.dataSerializer = dataSerializer;
    }

    @Override
    public void write(GetResponse response) throws SerializationException {
        dataSerializer.writeBytes(response.getContent());
        dataSerializer.flush();
    }

    @Override
    public void write(ListResponse response) throws SerializationException {
        dataSerializer.writeInt(response.getCount());
        dataSerializer.writeFileInfos(response.getFileInfos());
        dataSerializer.flush();
    }

    @Override
    public void write(SourcesResponse response) throws SerializationException {
        dataSerializer.writeInt(response.getSize());
        dataSerializer.writeAddresses(response.getClients());
        dataSerializer.flush();
    }

    @Override
    public void write(StatResponse response) throws SerializationException {
        dataSerializer.writeInt(response.getCount());
        dataSerializer.writeIntegers(response.getParts());
        dataSerializer.flush();
    }

    @Override
    public void write(UpdateResponse response) throws SerializationException {
        dataSerializer.writeBoolean(response.getStatus());
        dataSerializer.flush();
    }

    @Override
    public void write(UploadResponse response) throws SerializationException {
        dataSerializer.writeInt(response.getId());
        dataSerializer.flush();
    }
}
