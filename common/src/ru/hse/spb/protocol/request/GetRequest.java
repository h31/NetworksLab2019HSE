package ru.hse.spb.protocol.request;

import ru.hse.spb.protocol.SerializationException;

public class GetRequest implements AbstractRequest {
    public static final byte TYPE = 2;
    private final int id;
    private final int part;

    public GetRequest(int id, int part) {
        this.id = id;
        this.part = part;
    }

    @Override
    public byte getType() {
        return TYPE;
    }

    public int getId() {
        return id;
    }

    public int getPart() {
        return part;
    }

    @Override
    public void accept(RequestVisitor visitor) throws SerializationException {
        visitor.visitGetRequest(this);
    }
}
