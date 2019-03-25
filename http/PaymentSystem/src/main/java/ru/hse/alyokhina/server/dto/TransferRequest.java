package ru.hse.alyokhina.server.dto;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

import javax.annotation.Nonnull;


public class TransferRequest {
    private final String fromLogin;
    private final String toLogin;
    private final String fromPassword;
    private final long count;

    @JsonCreator
    public TransferRequest(@JsonProperty("fromLogin") @Nonnull final String fromLogin,
                           @JsonProperty("toLogin") @Nonnull final String toLogin,
                           @JsonProperty("fromPassword") @Nonnull final String fromPassword,
                           @JsonProperty("count") final long count) {
        this.fromLogin = fromLogin;
        this.toLogin = toLogin;
        this.fromPassword = fromPassword;
        this.count = count;
    }

    public String getFromLogin() {
        return fromLogin;
    }

    public String getToLogin() {
        return toLogin;
    }

    public String getFromPassword() {
        return fromPassword;
    }

    public long getCount() {
        return count;
    }
}
