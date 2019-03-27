package org.itmo.torrent.tracker;

import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.SeedInfo;
import org.itmo.torrent.State;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.network.Address;

import java.io.*;
import java.util.*;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.stream.Collectors;

public class TorrentTrackerState implements State {
    @NotNull
    private static final String STATE_FILE_NAME = ".torrent-tracker.stt";

    @NotNull
    private final Map<Integer, FileInfo> fileInfos = new HashMap<>();
    @NotNull
    private final Map<Integer, Set<SeedInfo>> fileSeeders = new HashMap<>();

    @NotNull
    @Override
    public String getStateFileName() {
        return STATE_FILE_NAME;
    }

    public synchronized int addNewFile(@NotNull final String fileName, final long fileSize) {
        final FileInfo fileInfo = new FileInfo(fileInfos.size(), fileName, fileSize);
        fileInfos.putIfAbsent(fileInfo.getId(), fileInfo);
        return fileInfo.getId();
    }

    @NotNull
    public synchronized List<FileInfo> getFileInfos() {
        return new CopyOnWriteArrayList<>(fileInfos.values());
    }

    public synchronized void addSeeder(final int fileId, @NotNull final Address seederAddress) {
        final Set<SeedInfo> seeders = fileSeeders.computeIfAbsent(fileId, (key) -> new HashSet<>());
        seeders.add(new SeedInfo(seederAddress));
    }

    @NotNull
    public synchronized Set<Address> getSeeders(final int fileId) {
        return fileSeeders.getOrDefault(fileId, Collections.emptySet()).stream()
                .filter(seedInfo -> !seedInfo.isExpired())
                .map(SeedInfo::getAddress)
                .collect(Collectors.toSet());
    }

    @Override
    public synchronized void toOutputStream(@NotNull final OutputStream out) throws IOException {
        try (DataOutputStream dataOut = new DataOutputStream(out)) {
            final Collection<FileInfo> fileInfos = this.fileInfos.values();
            dataOut.writeInt(fileInfos.size());
            for (FileInfo fileInfo : fileInfos) {
                dataOut.writeInt(fileInfo.getId());
                dataOut.writeUTF(fileInfo.getName());
                dataOut.writeLong(fileInfo.getSize());
            }
        }
    }

    @Override
    public synchronized void fromInputStream(@NotNull final InputStream in) throws IOException {
        try (DataInputStream dataIn = new DataInputStream(in)) {
            final int count = dataIn.readInt();
            for (int i = 0; i < count; i++) {
                final int fileId = dataIn.readInt();
                final String fileName = dataIn.readUTF();
                final long fileSize = dataIn.readLong();
                fileInfos.put(fileId, new FileInfo(fileId, fileName, fileSize));
            }
        }
    }
}
