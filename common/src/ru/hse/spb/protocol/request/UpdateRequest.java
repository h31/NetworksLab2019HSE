package ru.hse.spb.protocol.request;

import org.jetbrains.annotations.NotNull;

import java.util.List;

public class UpdateRequest implements AbstractRequest {
    public static final byte TYPE = 4;
    private final char clientPort;
    private final @NotNull List<Integer> ids;

    public UpdateRequest(int clientPort, @NotNull List<Integer> ids) {
        this((char) clientPort, ids);
    }

    public UpdateRequest(char clientPort, @NotNull List<Integer> ids) {
        this.clientPort = clientPort;
        this.ids = ids;
    }

    @Override
    public byte getType() {
        return TYPE;
    }

    public char getClientPort() {
        return clientPort;
    }

    public int getClientPortAsInt() {
        return (int) clientPort;
    }

    public int getCount() {
        return ids.size();
    }

    @NotNull
    public List<Integer> getIds() {
        return ids;
    }

    @Override
    public void accept(RequestVisitor visitor) {
        visitor.visitUpdateRequest(this);
    }
}
