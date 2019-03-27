package ru.hse.spb.protocol.request;

import ru.hse.spb.protocol.SerializationException;

public class ListRequest implements AbstractRequest {
    public static final byte TYPE = 1;

    @Override
    public byte getType() {
        return TYPE;
    }

    @Override
    public void accept(RequestVisitor visitor) throws SerializationException {
        visitor.visitListRequest(this);
    }
}
