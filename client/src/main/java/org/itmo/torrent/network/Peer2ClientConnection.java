package org.itmo.torrent.network;

import org.apache.commons.io.IOUtils;
import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.network.messages.client.Get;
import org.itmo.torrent.network.messages.client.Stat;

import java.io.IOException;
import java.net.Socket;

public class Peer2ClientConnection extends Connection {

    public Peer2ClientConnection(@NotNull final Socket socket) throws IOException {
        super(socket);
    }

    public void sendStatResponse(@NotNull final Stat.Response message) throws IOException {
        out.writeInt(message.getFileChunksIds().size());
        for (int filePartId : message.getFileChunksIds()) {
            out.writeInt(filePartId);
        }
        out.flush();
    }

    public void sendGetResponse(@NotNull final Get.Response message) throws IOException {
        out.writeInt(message.getCount());
        if (message.getFileChunkContent() != null) {
            IOUtils.copy(message.getFileChunkContent(), out);
        }
        out.flush();
    }

    @NotNull
    public Stat.Request receiveStatRequest() throws IOException {
        final int fileId = in.readInt();
        return new Stat.Request(fileId);
    }

    @NotNull
    public Get.Request receiveGetRequest() throws IOException {
        final int fileId = in.readInt();
        final int filePartId = in.readInt();
        return new Get.Request(fileId, filePartId);
    }
}
