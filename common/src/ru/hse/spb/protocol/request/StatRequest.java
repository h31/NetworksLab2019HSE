package ru.hse.spb.protocol.request;

import ru.hse.spb.protocol.SerializationException;

public class StatRequest implements AbstractRequest {
    public static final byte TYPE = 1;
    private final int id;

    public StatRequest(int id) {
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
        visitor.visitStatRequest(this);
    }
}
