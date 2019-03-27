package ru.hse.spb.protocol.request;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.SerializationException;

public class UploadRequest implements AbstractRequest {
    public static final byte TYPE = 2;
    private final @NotNull String name;
    private final long size;

    public UploadRequest(@NotNull String name, long size) {
        this.name = name;
        this.size = size;
    }

    @Override
    public byte getType() {
        return TYPE;
    }

    @NotNull
    public String getName() {
        return name;
    }

    public long getSize() {
        return size;
    }

    @Override
    public void accept(RequestVisitor visitor) throws SerializationException {
        visitor.visitUploadRequest(this);
    }
}
