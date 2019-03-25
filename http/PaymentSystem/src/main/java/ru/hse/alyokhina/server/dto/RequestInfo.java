package ru.hse.alyokhina.server.dto;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

import javax.annotation.Nonnull;

public class RequestInfo {
    private final String from;
    private final String key;
    private final long count;

    @JsonCreator
    public RequestInfo(@JsonProperty("from") @Nonnull final String from,
                       @JsonProperty("key") @Nonnull final String key,
                       @JsonProperty("count") final long count) {
        this.from = from;
        this.key = key;
        this.count = count;
    }

    public String getFrom() {
        return from;
    }

    public String getKey() {
        return key;
    }

    public long getCount() {
        return count;
    }
}
