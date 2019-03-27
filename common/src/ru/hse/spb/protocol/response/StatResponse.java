package ru.hse.spb.protocol.response;

import org.jetbrains.annotations.NotNull;

import java.util.List;

public class StatResponse implements AbstractResponse {
    private final int count;
    private final @NotNull List<Integer> parts;

    public StatResponse(int count, @NotNull List<Integer> parts) {
        this.count = count;
        this.parts = parts;
    }

    public int getCount() {
        return count;
    }

    @NotNull
    public List<Integer> getParts() {
        return parts;
    }
}
