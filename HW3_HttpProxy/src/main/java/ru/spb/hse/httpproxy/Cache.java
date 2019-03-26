package ru.spb.hse.httpproxy;

import org.jetbrains.annotations.NotNull;

import java.time.Duration;
import java.time.Instant;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;

public class Cache {
    private Duration expirationTime;
    private Map<String, CachedObject> cache;

    public Cache(Duration expirationTime, int cacheSize) {
        this.expirationTime = expirationTime;
        this.cache = Collections.synchronizedMap(new LinkedHashMap<String, CachedObject>() {
            @Override
            public boolean removeEldestEntry(Map.Entry eldest) {
                return size() > cacheSize;
            }
        });
    }

    public String getResponse(@NotNull String request) {
        if (!cache.containsKey(request)) {
            return null;
        }
        CachedObject element = cache.get(request);
        if (Duration.between(cache.get(request).wasCreated, Instant.now())
                .compareTo(expirationTime) > 0) {
            cache.remove(request);
            return null;
        }
        return element.response;
    }

    public void add(@NotNull String request, @NotNull String response) {
        cache.put(request, new CachedObject(response, Instant.now()));
    }

    private static class CachedObject {
        private String response;
        private Instant wasCreated;

        CachedObject(@NotNull String response, @NotNull Instant wasCreated) {
            this.response = response;
            this.wasCreated = wasCreated;
        }
    }
}
