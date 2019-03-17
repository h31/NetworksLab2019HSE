package ru.ifmo.torrent.tracker.state;

import ru.ifmo.torrent.tracker.TrackerConfig;

import java.util.Objects;

public class TimedSeedInfo {

    private final SeedInfo seedInfo;
    private final long creationTime;

    public TimedSeedInfo(SeedInfo seedInfo, long time) {
        this.seedInfo = seedInfo;
        this.creationTime = time;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        TimedSeedInfo that = (TimedSeedInfo) o;
        return Objects.equals(seedInfo, that.seedInfo);
    }

    @Override
    public int hashCode() {
        return Objects.hash(seedInfo);
    }

    public SeedInfo getSeedInfo() {
        return seedInfo;
    }

    public boolean notAlive(long currentTime) {
        return currentTime - creationTime >= TrackerConfig.TIMEOUT;
    }

}
