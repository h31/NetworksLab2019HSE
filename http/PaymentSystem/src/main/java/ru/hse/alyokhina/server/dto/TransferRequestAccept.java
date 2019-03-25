package ru.hse.alyokhina.server.dto;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;

import javax.annotation.Nonnull;

public class TransferRequestAccept {
    
    private final String login;
    private final String password;
    private final String key;
    private final ActionType actionType;

    @JsonCreator
    public TransferRequestAccept(@JsonProperty("login") @Nonnull final String login,
                                 @JsonProperty("password") @Nonnull final String password,
                                 @JsonProperty("key") @Nonnull final String key,
                                 @JsonProperty("actionType") @Nonnull ActionType actionType) {

        this.login = login;
        this.password = password;
        this.key = key;
        this.actionType = actionType;
    }

    public String getLogin() {
        return login;
    }

    public String getPassword() {
        return password;
    }

    public String getKey() {
        return key;
    }

    public ActionType getActionType() {
        return actionType;
    }
}
