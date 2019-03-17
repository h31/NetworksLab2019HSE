package ru.ifmo.torrent.client.leech;

import ru.ifmo.torrent.client.Client;
import ru.ifmo.torrent.client.ClientConfig;
import ru.ifmo.torrent.client.storage.LocalFilesManager;
import ru.ifmo.torrent.client.storage.PartsManager;
import ru.ifmo.torrent.tracker.state.SeedInfo;
import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.io.OutputStream;
import java.util.*;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;
import java.util.stream.Collectors;

public class Downloader implements Runnable, AutoCloseable {

    private static final int DOWNLOADS_LIMIT = ClientConfig.DOWNLOADS_LIMIT;

    private final ExecutorService pool = Executors.newFixedThreadPool(DOWNLOADS_LIMIT);
    private final LocalFilesManager filesManager;
    private final Client client;
    private final Set<FilePart> downloadingParts;
    private boolean shouldExit = false;

    public Downloader(LocalFilesManager filesManager, Client client) {
        this.filesManager = filesManager;
        downloadingParts = new HashSet<>();
        this.client = client;
    }

    @Override
    public void close() {
        shouldExit = true;
        pool.shutdown();
    }

    public void updateDownloads() {
        Set<FilePart> partsToDownload = filesManager.getFiles().stream().flatMap(
            f -> f.getMissingParts().stream()
                .map(p -> new FilePart(f.getFileId(), p))
                .filter(p -> !downloadingParts.contains(p)
                )
        ).collect(Collectors.toSet());

        Set<Integer> fileIds = partsToDownload.stream().map(FilePart::getFileId).collect(Collectors.toSet());

        Map<Integer, List<SeedInfo>> sourcesForFile = getSourcesForFiles(fileIds);
        partsToDownload.stream().filter(p -> !sourcesForFile.get(p.getFileId()).isEmpty())
            .limit(DOWNLOADS_LIMIT - downloadingParts.size())
            .forEach(p -> downloadPart(p, sourcesForFile.get(p.getFileId())));
    }

    private Map<Integer, List<SeedInfo>> getSourcesForFiles(Set<Integer> fileIds) {
        Map<Integer, List<SeedInfo>> sources = new HashMap<>();
        for (Integer fileId : fileIds) {
            try {
                sources.put(fileId, client.getFileSources(fileId));
            } catch (TorrentException e) {
                e.printStackTrace();
            }
        }
        return sources;
    }

    private void downloadPart(FilePart part, List<SeedInfo> sources) {
        if (!sources.isEmpty()) {
            try {
                pool.submit(new DownloadTask(part, sources));
            } catch (RejectedExecutionException e) {
                shouldExit = true;
            }
        }
    }

    @Override
    public void run() {
        while (!shouldExit) {
            try {
                Thread.sleep(ClientConfig.DOWNLOAD_RATE_SEC * 1000);
            } catch (InterruptedException e) {
                break;
            }
            updateDownloads();
        }
    }

    private class DownloadTask implements Runnable {

        private final FilePart part;
        private final List<SeedInfo> sources;
        private PartsManager partsManager = filesManager.getPartsManager();

        DownloadTask(FilePart part, List<SeedInfo> sources) {
            this.part = part;
            this.sources = sources;
            downloadingParts.add(part);
        }

        @Override
        public void run() {
            try {
                Optional<SeedInfo> maybeSource = getSource();
                if (!maybeSource.isPresent()) return;

                SeedInfo source = maybeSource.get();
                Leecher leecher = new Leecher(source.getPort(), source.getInetAddress());

                byte[] content = leecher.getPartContent(part.getFileId(), part.getPartNum());

                try (OutputStream out = partsManager.getForWriting(part.getFileId(), part.getPartNum())) {
                    out.write(content);
                    out.flush();
                }

                filesManager.addReadyPartOfFile(part.getFileId(), part.getPartNum());
                downloadingParts.remove(part);

            } catch (IOException e) {
                System.err.printf("error while downloading file with id %d%n", part.getFileId());
                e.printStackTrace();
            } catch (TorrentException e) {
                System.err.println(e.getMassage());
                e.getException().printStackTrace();
            }
        }

        private Optional<SeedInfo> getSource() throws TorrentException {
            for (SeedInfo s : sources) {
                Leecher leecher = new Leecher(s.getPort(), s.getInetAddress());
                List<Integer> availableParts = leecher.getAvailableParts(part.getFileId());
                if (availableParts.contains(part.getPartNum())) {
                    return Optional.of(s);
                }
            }
            return Optional.empty();
        }
    }

}