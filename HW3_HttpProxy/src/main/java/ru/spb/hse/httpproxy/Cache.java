package ru.spb.hse.httpproxy;

import org.jetbrains.annotations.NotNull;

import java.time.Duration;
import java.time.Instant;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.logging.Logger;

public class Cache {
    private static final Logger LOGGER = Logger.getLogger("Cache");
    private Duration expirationTime;
    private Map<Integer, CachedObject> cache;

    public Cache(Duration expirationTime, int cacheSize) {
        this.expirationTime = expirationTime;
        this.cache = Collections.synchronizedMap(new LinkedHashMap<Integer,
                CachedObject>() {
            @Override
            public boolean removeEldestEntry(Map.Entry eldest) {
                return size() > cacheSize;
            }
        });
    }

    public synchronized byte[] getResponse(@NotNull byte[] request) {
        if (!contains(request)) {
            return null;
        }
        return cache.get(Arrays.hashCode(request)).response;
    }

    public synchronized boolean contains(@NotNull byte[] request) {
        int hash = Arrays.hashCode(request);
        if (!cache.containsKey(hash)) {
            LOGGER.info("The request is not in " +
                    "cache");
            return false;
        }
        if (Duration.between(cache.get(hash).wasCreated, Instant.now())
                .compareTo(expirationTime) > 0) {
            LOGGER.info("The request " + new String(request) + " is outdated");
            cache.remove(hash);
            return false;
        }
        return true;
    }

    public synchronized void add(@NotNull byte[] request, @NotNull byte[] response) {
        if (contains(request)) {
            return;
        }
        int hash = Arrays.hashCode(request);
        cache.put(hash, new CachedObject(response));
    }

    private static class CachedObject {
        private byte[] response;
        private Instant wasCreated;

        CachedObject(@NotNull byte[] response) {
            this.response = response;
            this.wasCreated = Instant.now();
        }
    }
}
