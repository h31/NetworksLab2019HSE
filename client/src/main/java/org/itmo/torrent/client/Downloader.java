package org.itmo.torrent.client;

import org.apache.commons.io.IOUtils;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.filesystem.FileSeedInfo;
import org.itmo.torrent.network.Address;

import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RecursiveTask;

public class Downloader implements AutoCloseable {
    @NotNull
    private final TorrentClient client;
    @NotNull
    private final ExecutorService executor = Executors.newWorkStealingPool();

    public Downloader(@NotNull final TorrentClient client) {
        this.client = client;
    }

    void download(final int fileId) {
        executor.submit(new DownloadFileTask(fileId));
    }

    @Override
    public void close() {
        executor.shutdown();
    }

    private class DownloadFileTask implements Runnable {
        private final int fileId;

        DownloadFileTask(final int fileId) {
            this.fileId = fileId;
        }

        @Override
        public void run() {
            final FileSeedInfo fileInfo = getFileSeedInfo();
            if (fileInfo == null) {
                return;
            }

            final Map<Integer, Set<Address>> fileChunkSeeders = getFileChunksSeeders();
            if (fileChunkSeeders == null) {
                return;
            }

            try (RandomAccessFile out = new RandomAccessFile(fileInfo.getFile(), "rw")) {
                out.setLength(fileInfo.getSize());
                client.getState().addFileInfo(fileInfo);
                final List<DownloadFileChunkTask> subTasks = fork(fileInfo, fileChunkSeeders);
                join(fileInfo, out, subTasks);
            } catch (IOException ignore) {
                // TODO: proper handling
            }
        }

        @Nullable
        private Map<Integer, Set<Address>> getFileChunksSeeders() {
            // TODO: fix bottleneck
            final Map<Integer, Set<Address>> fileChunkSeeders = new HashMap<>();
            try {
                for (Address source : client.executeSources(fileId)) {
                    for (int chunksId : client.executeStat(source, fileId)) {
                        fileChunkSeeders.putIfAbsent(chunksId, new HashSet<>());
                        fileChunkSeeders.get(chunksId).add(source);
                    }
                }
            } catch (IOException e) {
                System.err.println(e.getMessage());
                return null;
            }

            return fileChunkSeeders;
        }

        @NotNull
        private List<DownloadFileChunkTask> fork(@NotNull final FileSeedInfo fileInfo,
                                                 @NotNull final Map<Integer, Set<Address>> fileChunkSeeders) {
            return new ArrayList<DownloadFileChunkTask>() {{
                for (int fileChunkId = 0; fileChunkId < fileInfo.getChunksNumber(); fileChunkId++) {
                    if (!fileInfo.isChunkAvailable(fileChunkId)) {
                        final DownloadFileChunkTask task = new DownloadFileChunkTask(fileId, fileChunkId,
                                fileChunkSeeders.getOrDefault(fileChunkId, Collections.emptySet()));
                        task.fork();
                        add(task);
                    }
                }
            }};
        }

        private void join(@NotNull final FileSeedInfo fileInfo, @NotNull final RandomAccessFile out,
                          @NotNull final List<DownloadFileChunkTask> subTasks) throws IOException {
            for (DownloadFileChunkTask task : subTasks) {
                final InputStream fileChunkContent = task.join();
                out.seek(task.fileChunkId * FileInfo.FILE_CHUNK_SIZE);
                out.write(IOUtils.toByteArray(fileChunkContent));
                fileInfo.setChunkAvailable(task.fileChunkId);
            }
        }

        @Nullable
        private FileSeedInfo getFileSeedInfo() {
            FileSeedInfo fileInfo = client.getState().getFileInfo(fileId);
            if (fileInfo == null) {
                try {
                    FileInfo tempFileInfo = client.executeList().stream()
                            .filter(info -> info.getId() == fileId)
                            .findAny()
                            .orElse(null);
                    fileInfo = tempFileInfo != null ? new FileSeedInfo(tempFileInfo) : null;
                } catch (IOException ignored) {
                }
            }

            return fileInfo;
        }
    }

    private class DownloadFileChunkTask extends RecursiveTask<InputStream> {
        private final int fileId;
        private final int fileChunkId;
        @NotNull
        private final Set<Address> seeders;

        DownloadFileChunkTask(final int fileId, final int fileChunkId,
                              @NotNull final Set<Address> seeders) {
            this.fileId = fileId;
            this.fileChunkId = fileChunkId;
            this.seeders = seeders;
        }

        @Nullable
        @Override
        protected InputStream compute() {
            for (Address address : seeders) {
                InputStream fileChunkContent = null;
                try {
                    fileChunkContent = client.executeGet(address, fileId, fileChunkId);
                } catch (IOException ignored) {
                }
                if (fileChunkContent != null) {
                    return fileChunkContent;
                }
            }

            return null;
        }
    }
}

