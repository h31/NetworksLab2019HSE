package org.itmo.torrent.network.messages.tracker;

import org.itmo.torrent.network.messages.Message;
import org.jetbrains.annotations.NotNull;

public interface Upload extends Message {
    byte ID = 2;

    class Request implements Message {
        @NotNull
        private final String fileName;
        private final long fileSize;

        public Request(@NotNull final String fileName, final long fileSize) {
            this.fileName = fileName;
            this.fileSize = fileSize;
        }

        @NotNull
        public String getFileName() {
            return fileName;
        }

        public long getFileSize() {
            return fileSize;
        }

        @NotNull
        @Override
        public String toString() {
            return String.format("UploadRequest{fileName=%s, fileSize=%d}", fileName, fileSize);
        }
    }

    class Response implements Message {
        private final int fileId;

        public Response(final int fileId) {
            this.fileId = fileId;
        }

        public int getFileId() {
            return fileId;
        }

        @NotNull
        @Override
        public String toString() {
            return String.format("UploadResponse{fileId=%d}", fileId);
        }
    }
}
