package org.itmo.torrent.client;

import org.itmo.torrent.State;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.itmo.torrent.filesystem.FileSeedInfo;

import java.io.*;
import java.util.*;

public class TorrentClientState implements State {
    @NotNull
    private static final String STATE_FILE_NAME = ".torrent-client.stt";

    @NotNull
    private final Map<Integer, FileSeedInfo> fileInfos = new HashMap<>();

    @NotNull
    @Override
    public String getStateFileName() {
        return STATE_FILE_NAME;
    }

    public synchronized void addFileInfo(@NotNull final FileSeedInfo fileInfo) {
        fileInfos.put(fileInfo.getId(), fileInfo);
    }

    @Nullable
    public FileSeedInfo getFileInfo(final int fileId) {
        return fileInfos.get(fileId);
    }

    @NotNull
    public synchronized Set<Integer> getFilesIds() {
        return fileInfos.keySet();
    }

    @Override
    public synchronized void toOutputStream(@NotNull final OutputStream out) throws IOException {
        try (ObjectOutputStream objOut = new ObjectOutputStream(out)) {
            final Collection<FileSeedInfo> fileInfos = this.fileInfos.values();
            objOut.writeInt(fileInfos.size());
            for (FileSeedInfo fileInfo : fileInfos) {
                objOut.writeInt(fileInfo.getId());
                objOut.writeUTF(fileInfo.getName());
                objOut.writeLong(fileInfo.getSize());
                objOut.writeUTF(fileInfo.getFile().getAbsolutePath());
                objOut.writeObject(fileInfo.getAvailableChunks());
            }
        }
    }

    @Override
    public synchronized void fromInputStream(@NotNull final InputStream in)
            throws IOException, ClassNotFoundException {
        try (ObjectInputStream objIn = new ObjectInputStream(in)) {
            final int count = objIn.readInt();
            for (int i = 0; i < count; i++) {
                final int fileId = objIn.readInt();
                final String fileName = objIn.readUTF();
                final long fileSize = objIn.readLong();
                final File file = new File(objIn.readUTF());
                final BitSet availableChunks = (BitSet) objIn.readObject();
                fileInfos.put(fileId, new FileSeedInfo(fileId, fileName, fileSize, file, availableChunks));
            }
        }
    }
}
