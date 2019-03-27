package ru.hse.spb.protocol.request;

import ru.hse.spb.protocol.SerializationException;

public class SourcesRequest implements AbstractRequest {
    public static final byte TYPE = 3;
    private final int id;

    public SourcesRequest(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    @Override
    public byte getType() {
        return TYPE;
    }

    @Override
    public void accept(RequestVisitor visitor) throws SerializationException {
        visitor.visitSourcesRequest(this);
    }
}
