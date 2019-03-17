package ru.ifmo.torrent.client.storage;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.RandomAccessFile;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;

public class PartsManager {

    private final Path storage;

    public PartsManager(Path storage) {
        this.storage = storage;
        if (Files.notExists(storage)) {
            storage.toFile().mkdirs();
        }
    }

    public void storeSplitted(LocalFileReference reference, Path targetFile) throws IOException {
        try (InputStream is = Files.newInputStream(targetFile)) {
            for (int i = 0; i < reference.getNumberOfParts(); i++) {
                int partSize = reference.getBlockSizeForPart(i);
                byte[] buf = new byte[partSize];
                int totalReaded = 0;
                try (OutputStream out = getForWriting(reference.getFileId(), i)) {
                    while (totalReaded != partSize) {
                        int readed = is.read(buf);
                        out.write(buf, totalReaded, readed);
                        totalReaded += readed;
                    }
                }
            }
        }
    }

    public void mergeSplitted(int fileId, long size, Path targetFile) throws IOException {
        Path fileDir = storage.resolve(String.valueOf(fileId));
        List<Path> parts = Files.list(fileDir)
            .sorted(Comparator.comparing(this::parsePartName))
            .collect(Collectors.toList());
        if (Files.notExists(targetFile)) {
            Files.createDirectories(targetFile.getParent());
            try (RandomAccessFile randomAccessFile = new RandomAccessFile(targetFile.toFile(), "rw")) {
                randomAccessFile.setLength(size);
            }
        }
        OutputStream out = Files.newOutputStream(targetFile, StandardOpenOption.TRUNCATE_EXISTING);
        for (Path p : parts) {
            Files.copy(p, out);
        }
    }

    private int parsePartName(Path path) {
        return Integer.parseInt(path.getFileName().toString());
    }

    public OutputStream getForWriting(int fileId, int part) throws IOException {
        Path partFile = storage.resolve(String.valueOf(fileId)).resolve(String.valueOf(part));
        if (Files.notExists(partFile)) {
            Files.createDirectories(partFile.getParent());
            Files.createFile(partFile);
        }
        return Files.newOutputStream(partFile, StandardOpenOption.TRUNCATE_EXISTING);
    }

    public InputStream getForReading(int fileId, int part) throws IOException {
        Path partFile = storage.resolve(String.valueOf(fileId)).resolve(String.valueOf(part));
        return Files.newInputStream(partFile);
    }

    public boolean fileIsPresent(int fileId) {
        return Files.exists(storage.resolve(String.valueOf(fileId)));
    }

}
