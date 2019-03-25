package ru.hse.alyokhina.server.dto;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;

import javax.annotation.Nonnull;

@JsonInclude(JsonInclude.Include.NON_NULL)
public class DefaultResponse {
    private final Status status;
    private final String description;
    private final String entityId;

    @JsonCreator
    public DefaultResponse(@JsonProperty("status") @Nonnull final Status status,
                           @JsonProperty("description") final String description,
                           @JsonProperty("entityId") final String entityId) {
        this.status = status;
        this.description = description;
        this.entityId = entityId;
    }

    public Status getStatus() {
        return status;
    }

    public String getDescription() {
        return description;
    }

    public String getEntityId() {
        return entityId;
    }
}
