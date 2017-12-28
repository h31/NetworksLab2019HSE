package org.itmo.torrent.network.messages.client;

import org.itmo.torrent.network.messages.Message;
import org.jetbrains.annotations.NotNull;

import java.util.Set;

public interface Stat extends Message {
    byte ID = 1;

    class Request implements Stat {
        private final int fileId;

        public Request(final int fileId) {
            this.fileId = fileId;
        }

        public int getFileId() {
            return fileId;
        }

        @NotNull
        @Override
        public String toString() {
            return String.format("StatRequest{fileId=%d}", fileId);
        }
    }

    class Response implements Stat {
        @NotNull
        private final Set<Integer> fileChunksIds;

        public Response(@NotNull final Set<Integer> fileChunksIds) {
            this.fileChunksIds = fileChunksIds;
        }

        @NotNull
        public Set<Integer> getFileChunksIds() {
            return fileChunksIds;
        }

        @NotNull
        @Override
        public String toString() {
            return String.format("StatResponse{count=%d, fileChunksIds=%s}",
                    fileChunksIds.size(), fileChunksIds);
        }
    }
}
