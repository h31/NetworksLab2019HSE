package ru.hse.alyokhina.server.dto;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

import javax.annotation.Nonnull;

public class UserInfo {
    private final String login;
    private final String password;

    @JsonCreator
    public UserInfo(@JsonProperty("login") @Nonnull final String login,
                    @JsonProperty("password") @Nonnull final String password) {

        this.login = login;
        this.password = password;
    }

    public String getLogin() {
        return login;
    }

    public String getPassword() {
        return password;
    }
}
