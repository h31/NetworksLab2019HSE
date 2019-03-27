package ru.hse.spb.protocol.response;

import org.jetbrains.annotations.NotNull;

public class GetResponse implements AbstractResponse {
    private final @NotNull byte[] content;

    public GetResponse(byte[] content) {
        this.content = content;
    }


    @NotNull
    public byte[] getContent() {
        return content;
    }
}