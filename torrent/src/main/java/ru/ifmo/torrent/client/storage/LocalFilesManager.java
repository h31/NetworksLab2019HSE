package ru.ifmo.torrent.client.storage;

import ru.ifmo.torrent.client.ClientConfig;
import ru.ifmo.torrent.util.StoredState;
import ru.ifmo.torrent.util.TorrentException;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class LocalFilesManager implements StoredState {

    private ConcurrentHashMap<Integer, LocalFileReference> localFiles = new ConcurrentHashMap<>();
    private PartsManager partsManager;
    private final Path metaFile;
    private final Path downloadDir;

    public LocalFilesManager(Path downloadDir, Path metaFile, Path partsStorage) throws IOException, TorrentException {
        this.metaFile = metaFile;
        this.downloadDir = downloadDir;
        if (Files.notExists(metaFile)) {
            metaFile.getParent().toFile().mkdirs();
            Files.createFile(metaFile);
        }
        partsManager = new PartsManager(partsStorage);
        restoreFromFile();
    }

    public void addLocalFile(String name, int fileId, long size) {
        int partsNum = getPartsNumber(size);
        localFiles.putIfAbsent(fileId, LocalFileReference.createFull(name, fileId, size, partsNum));
    }

    public void addNotDownloadedFile(String name, int fileId, long size) {
        LocalFileReference reference = LocalFileReference.createEmpty(name, fileId, size, getPartsNumber(size));
        localFiles.put(fileId, reference);
    }

    public void addReadyPartOfFile(int fileId, int part) throws IOException {
        getOrThrow(fileId).addReadyPart(part);
        if (getOrThrow(fileId).getMissingParts().isEmpty()) {
            LocalFileReference reference = localFiles.get(fileId);
            String fileName = reference.getName();
            long fileSize = reference.getSize();
            partsManager.mergeSplitted(fileId, fileSize, downloadDir.resolve(fileName));
        }
    }

    private LocalFileReference getOrThrow(int fileId) {
        return Objects.requireNonNull(localFiles.get(fileId), "no file with id " + fileId);
    }

    public LocalFileReference getFileReference(int fileId) {
        return getOrThrow(fileId);
    }

    public List<LocalFileReference> getFiles() {
        return new ArrayList<>(localFiles.values());
    }

    private int getPartsNumber(long size) {
        return (int) Math.ceil(size / (double) ClientConfig.FILE_PART_SIZE);
    }

    @Override
    public void storeToFile() throws TorrentException {
        try (DataOutputStream out = new DataOutputStream(Files.newOutputStream(metaFile))) {
            out.writeInt(localFiles.size());
            for (LocalFileReference file : localFiles.values()) {
                file.write(out);
            }
            out.flush();
        } catch (IOException e) {
            throw new TorrentException("cannot save local files manager state", e);
        }
    }

    @Override
    public void restoreFromFile() throws TorrentException {
        try {
            if (Files.size(metaFile) == 0) return;
            localFiles = new ConcurrentHashMap<>();
            try (DataInputStream in = new DataInputStream(Files.newInputStream(metaFile))) {
                int numOfLocalFiles = in.readInt();
                for (int i = 0; i < numOfLocalFiles; i++) {
                    LocalFileReference file = LocalFileReference.readFrom(in);
                    localFiles.put(file.getFileId(), file);
                }
            }
        } catch (IOException e) {
            throw new TorrentException("cannot restore local files manager state", e);
        }
    }

    public PartsManager getPartsManager() {
        return partsManager;
    }

    public void addFileToStorageAsParts(int fileId, Path file) throws IOException {
        long size = Files.size(file);
        LocalFileReference reference = LocalFileReference.createEmpty(file.getFileName().toString(), fileId, size, getPartsNumber(size));
        partsManager.storeSplitted(reference, file);

    }
}
