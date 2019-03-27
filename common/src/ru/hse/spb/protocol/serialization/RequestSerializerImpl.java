package ru.hse.spb.protocol.serialization;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;
import ru.hse.spb.protocol.request.*;

public class RequestSerializerImpl implements RequestSerializer {
    private final @NotNull DataSerializer dataSerializer;

    protected RequestSerializerImpl(@NotNull DataSerializer dataSerializer) {
        this.dataSerializer = dataSerializer;
    }

    private void writeId(AbstractRequest request) throws SerializationException {
        dataSerializer.writeByte(request.getType());
        dataSerializer.flush();
    }

    @Override
    public void write(GetRequest request) throws SerializationException {
        writeId(request);
        dataSerializer.writeInt(request.getId());
        dataSerializer.writeInt(request.getPart());
        dataSerializer.flush();
    }

    @Override
    public void write(ListRequest request) throws SerializationException {
        writeId(request);
        dataSerializer.flush();
    }

    @Override
    public void write(SourcesRequest request) throws SerializationException {
        writeId(request);
        dataSerializer.writeInt(request.getId());
        dataSerializer.flush();
    }

    @Override
    public void write(StatRequest request) throws SerializationException {
        writeId(request);
        dataSerializer.writeInt(request.getId());
        dataSerializer.flush();
    }

    @Override
    public void write(UpdateRequest request) throws SerializationException {
        writeId(request);
        dataSerializer.writeChar(request.getClientPort());
        dataSerializer.writeInt(request.getCount());
        dataSerializer.writeIntegers(request.getIds());
        dataSerializer.flush();
    }

    @Override
    public void write(UploadRequest request) throws SerializationException {
        writeId(request);
        dataSerializer.writeString(request.getName());
        dataSerializer.writeLong(request.getSize());
        dataSerializer.flush();
    }
}
