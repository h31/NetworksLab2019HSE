package ru.hse.spb.protocol.response;

public class UpdateResponse implements AbstractResponse {
    private final boolean status;

    public UpdateResponse(boolean status) {
        this.status = status;
    }

    public boolean getStatus() {
        return status;
    }
}
