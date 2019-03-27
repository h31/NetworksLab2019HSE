package ru.hse.spb.protocol.response;

public class UploadResponse implements AbstractResponse {
    private final int id;

    public UploadResponse(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }
}
