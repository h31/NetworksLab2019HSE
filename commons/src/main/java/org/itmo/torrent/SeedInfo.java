package org.itmo.torrent;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.itmo.torrent.network.Address;
import org.itmo.torrent.network.messages.tracker.Update;

import java.util.Objects;

public class SeedInfo {
    @NotNull
    private final Address address;

    private volatile long lastUpdate ;

    public SeedInfo(@NotNull final Address address) {
        this.address = address;
        update();
    }

    public void update() {
        lastUpdate = System.currentTimeMillis();
    }

    @NotNull
    public Address getAddress() {
        return address;
    }

    public boolean isExpired() {
        return System.currentTimeMillis() - lastUpdate > Update.UPDATE_PERIOD * 2;
    }

    @Override
    public boolean equals(@Nullable final Object object) {
        if (this == object) {
            return true;
        }

        if (object == null || getClass() != object.getClass()) {
            return false;
        }

        final SeedInfo seedInfo = (SeedInfo) object;
        return Objects.equals(getAddress(), seedInfo.getAddress());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getAddress());
    }
}
