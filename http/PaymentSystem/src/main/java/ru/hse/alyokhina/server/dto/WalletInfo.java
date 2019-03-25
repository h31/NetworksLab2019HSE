package ru.hse.alyokhina.server.dto;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

import javax.annotation.Nonnull;

public class WalletInfo {
    private final String login;
    private final Long count;

    @JsonCreator
    public WalletInfo(@JsonProperty("login") @Nonnull final String login,
                    @JsonProperty("count") @Nonnull final Long count) {

        this.login = login;
        this.count = count;
    }

    public String getLogin() {
        return login;
    }

    public Long getCount() {
        return count;
    }
}
