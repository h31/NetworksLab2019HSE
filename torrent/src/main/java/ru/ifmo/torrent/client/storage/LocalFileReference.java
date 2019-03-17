package ru.ifmo.torrent.client.storage;

import ru.ifmo.torrent.client.ClientConfig;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public class LocalFileReference {

    private final int fileId;
    private final long size;
    private final int numberOfParts;
    private final Set<Integer> readyParts;
    private final String name;

    private LocalFileReference(String name, int fileId, long size, int numberOfParts, Set<Integer> readyParts) {
        this.fileId = fileId;
        this.name = name;
        this.numberOfParts = numberOfParts;
        this.readyParts = readyParts;
        this.size = size;
    }

    public static LocalFileReference createEmpty(String name, int fileId, long size, int numberOfParts) {
        return new LocalFileReference(name, fileId, size, numberOfParts, new HashSet<>());
    }

    public static LocalFileReference createFull(String name, int fileId, long size, int numberOfParts) {
        Set<Integer> readyParts = IntStream.range(0, numberOfParts).boxed().collect(Collectors.toSet());
        return new LocalFileReference(name, fileId, size, numberOfParts, readyParts);
    }

    public int getFileId() {
        return fileId;
    }

    public int getNumberOfParts() {
        return numberOfParts;
    }

    public List<Integer> getReadyParts() {
        return new ArrayList<>(readyParts);
    }

    public String getName() {
        return name;
    }

    public long getSize() {
        return size;
    }

    public void addReadyPart(int part) {
        if (part < numberOfParts) {
            readyParts.add(part);
        }
    }

    public static LocalFileReference readFrom(DataInputStream in) throws IOException {
        int id = in.readInt();
        long size = in.readLong();
        String name = in.readUTF();
        int numOfParts = in.readInt();
        int numOfReadyParts = in.readInt();
        Set<Integer> readyParts = new HashSet<>();
        for (int i = 0; i < numOfReadyParts; i++) {
            int part = in.readInt();
            readyParts.add(part);
        }
        return new LocalFileReference(name, id, size, numOfParts, readyParts);
    }

    public void write(DataOutputStream out) throws IOException {
        out.writeInt(fileId);
        out.writeLong(size);
        out.writeUTF(name);
        out.writeInt(numberOfParts);
        out.writeInt(readyParts.size());
        for (Integer part : readyParts) {
            out.writeInt(part);
        }
        out.flush();
    }

    public List<Integer> getMissingParts() {
        return IntStream.range(0, numberOfParts).boxed()
            .filter(i -> !readyParts.contains(i))
            .collect(Collectors.toList());
    }

    public int getBlockSizeForPart(int part) {
        int fullPartSize = ClientConfig.FILE_PART_SIZE;
        return (part + 1) < numberOfParts ? fullPartSize : (int) (size - (numberOfParts - 1) * fullPartSize);
    }

}
