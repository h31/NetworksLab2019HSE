package ru.hse.spb.protocol.response;

import org.jetbrains.annotations.NotNull;

import java.net.InetSocketAddress;
import java.util.List;

public class SourcesResponse implements AbstractResponse {
    private final @NotNull List<InetSocketAddress> clients;

    public SourcesResponse(@NotNull List<InetSocketAddress> clients) {
        this.clients = clients;
    }

    public int getSize() {
        return clients.size();
    }

    @NotNull
    public List<InetSocketAddress> getClients() {
        return clients;
    }
}
