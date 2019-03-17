package ru.ifmo.torrent.tracker.state;

import ru.ifmo.torrent.tracker.TrackerConfig;
import ru.ifmo.torrent.util.StoredState;
import ru.ifmo.torrent.util.TorrentException;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Instant;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.stream.Collectors;

public class TrackerState implements StoredState {

    private final ScheduledExecutorService pool;
    private final ConcurrentHashMap<Integer, FileInfo> availableFiles = new ConcurrentHashMap<>();
    private final ConcurrentHashMap<Integer, Set<TimedSeedInfo>> sources = new ConcurrentHashMap<>();

    private final Path metaFile;

    public TrackerState(Path metaFile) throws TorrentException, IOException {
        this.metaFile = metaFile;
        if (Files.notExists(metaFile)) {
            metaFile.getParent().toFile().mkdirs();
            Files.createFile(metaFile);
        }
        pool = Executors.newScheduledThreadPool(1);
        pool.scheduleAtFixedRate(this::updateSeedList, 0, TrackerConfig.UPDATE_RATE_SEC, TimeUnit.SECONDS);
        restoreFromFile();
    }

    public synchronized int addFile(String name, long size) {
        int id = generateId();
        availableFiles.put(id, new FileInfo(id, name, size));
        sources.put(id, Collections.synchronizedSet(new HashSet<>()));
        return id;
    }

    public synchronized List<FileInfo> getAvailableFiles() {
        return new ArrayList<>(availableFiles.values());
    }

    private synchronized int generateId() {
        return availableFiles.size() + 1;
    }

    public synchronized void addNewSeedIfAbsent(int fileId, SeedInfo source) {
        long currentTime = Instant.now().toEpochMilli();
        sources.computeIfAbsent(fileId, id ->
            Collections.synchronizedSet(new HashSet<>()))
            .add(new TimedSeedInfo(source, currentTime));
    }

    public synchronized List<SeedInfo> getSources(int fileId) {
        return sources.getOrDefault(fileId, new HashSet<>())
            .stream()
            .map(TimedSeedInfo::getSeedInfo)
            .collect(Collectors.toList());
    }

    @Override
    public synchronized void storeToFile() throws TorrentException {
        pool.shutdown();
        try (DataOutputStream out = new DataOutputStream(Files.newOutputStream(metaFile))) {
            out.writeInt(availableFiles.size());
            for (FileInfo info : availableFiles.values()) {
                info.write(out);
            }
            out.flush();
        } catch (IOException e) {
            throw new TorrentException("cannot save tracker state", e);
        }
    }

    @Override
    public void restoreFromFile() throws TorrentException {
        try {
            if (Files.size(metaFile) == 0) return;
            try (DataInputStream in = new DataInputStream(Files.newInputStream(metaFile))) {
                int filesNumber = in.readInt();
                for (int i = 0; i < filesNumber; ++i) {
                    FileInfo fileInfo = FileInfo.readFrom(in);
                    availableFiles.put(fileInfo.getId(), fileInfo);
                }
            }
        } catch (IOException e) {
            throw new TorrentException("cannot restore torrent state", e);
        }
    }

    private void updateSeedList() {
        long currentTime = Instant.now().toEpochMilli();
        for (Map.Entry<Integer, Set<TimedSeedInfo>> fileToSources : sources.entrySet()) {

            Set<TimedSeedInfo> values = fileToSources.getValue();
            synchronized (values) {
                values.removeIf(s -> s.notAlive(currentTime));
            }
        }
    }

}
