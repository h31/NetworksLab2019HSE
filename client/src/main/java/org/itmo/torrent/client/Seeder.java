package org.itmo.torrent.client;

import org.itmo.torrent.AbstractServer;
import org.itmo.torrent.network.Peer2ClientConnection;
import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.filesystem.FileSeedInfo;
import org.itmo.torrent.network.messages.client.Get;
import org.itmo.torrent.network.messages.client.Stat;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.net.Socket;
import java.util.Collections;
import java.util.Set;

public class Seeder extends AbstractServer {
    @NotNull
    private final TorrentClientState state;

    public Seeder(@NotNull final TorrentClientState state, final short port) throws IOException {
        super(port);
        this.state = state;
    }

    @NotNull
    @Override
    protected Runnable getConnectionHandler(@NotNull final Socket clientSocket) {
        return new ConnectionHandler(clientSocket);
    }

    private class ConnectionHandler implements Runnable {
        @NotNull
        final Socket socket;

        public ConnectionHandler(@NotNull final Socket socket) {
            this.socket = socket;
        }

        @Override
        public void run() {
            try (Peer2ClientConnection connection = new Peer2ClientConnection(socket)) {
                int requestType = connection.receiveRequestType();
                switch (requestType) {
                    case Stat.ID:
                        handleStatRequest(connection);
                        break;
                    case Get.ID:
                        handleGetRequest(connection);
                        break;
                    default:
                        throw new IOException("Unknown request type");
                }
            } catch (Exception e) {
                System.err.println(e.getMessage());
            }
        }

        private void handleStatRequest(@NotNull final Peer2ClientConnection connection)
                throws IOException {
            final Stat.Request request = connection.receiveStatRequest();
            final FileSeedInfo fileInfo = state.getFileInfo(request.getFileId());
            final Set<Integer> availableChunks = fileInfo != null
                    ? fileInfo.getSetAvailableChunks()
                    : Collections.emptySet();
            final Stat.Response response = new Stat.Response(availableChunks);
            connection.sendStatResponse(response);
        }

        private void handleGetRequest(@NotNull final Peer2ClientConnection connection)
                throws IOException {
            final Get.Request request = connection.receiveGetRequest();
            final FileSeedInfo fileInfo = state.getFileInfo(request.getFileId());

            if (fileInfo == null || !fileInfo.isChunkAvailable(request.getFileChunkId())) {
                final Get.Response response = new Get.Response(null, -1);
                connection.sendGetResponse(response);
                return;
            }

            final byte[] fileChunkContent;
            try (RandomAccessFile file = new RandomAccessFile(fileInfo.getFile(), "rw")) {
                file.seek(FileInfo.FILE_CHUNK_SIZE * request.getFileChunkId());
                fileChunkContent = new byte[(int) fileInfo.getChunkSize(request.getFileChunkId())];
                file.readFully(fileChunkContent);
            }

            final Get.Response response = new Get.Response(
                    new ByteArrayInputStream(fileChunkContent), fileChunkContent.length);
            connection.sendGetResponse(response);
        }
    }
}
