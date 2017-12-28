package org.itmo.torrent.network;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import java.util.Arrays;
import java.util.Objects;

public class Address {
    public static final int IP_SIZE = 4;

    @NotNull
    private final byte[] ip;
    private final short port;

    public Address(@NotNull final byte[] ip, final short port) {
        if (ip.length != IP_SIZE) {
            throw new IllegalArgumentException("Invalid IP size");
        }

        this.ip = ip;
        this.port = port;
    }

    @NotNull
    public byte[] getIp() {
        return ip;
    }

    @NotNull
    public String getHost() {
        return String.format("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    }

    public short getPort() {
        return port;
    }

    @NotNull
    @Override
    public String toString() {
        return String.format("Address{ip=%s, port=%d)", getHost(), getPort());
    }

    @Override
    public boolean equals(@Nullable final Object object) {
        if (this == object) {
            return true;
        }

        if (object == null || getClass() != object.getClass()) {
            return false;
        }

        final Address address = (Address) object;
        return getPort() == address.getPort() && Arrays.equals(getIp(), address.getIp());
    }

    @Override
    public int hashCode() {
        return 31 * Objects.hash(getPort()) + Arrays.hashCode(getIp());
    }
}
