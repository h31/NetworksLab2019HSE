package org.itmo.torrent.network;

import org.apache.commons.io.input.BoundedInputStream;
import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.network.messages.client.Get;
import org.itmo.torrent.network.messages.client.Stat;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.util.HashSet;
import java.util.Set;

public class Client2PeerConnection extends Connection {

    public Client2PeerConnection(@NotNull final Socket socket) throws IOException {
        super(socket);
    }

    public void sendStatRequest(@NotNull final Stat.Request message) throws IOException {
        out.writeByte(Stat.ID);
        out.writeLong(message.getFileId());
        out.flush();
    }

    public void sendGetRequest(@NotNull final Get.Request message) throws IOException {
        out.writeByte(Get.ID);
        out.writeInt(message.getFileId());
        out.writeInt(message.getFileChunkId());
        out.flush();
    }

    @NotNull
    public Stat.Response receiveStatResponse() throws IOException {
        final int count = in.readInt();
        final Set<Integer> filePartsIds = new HashSet<Integer>() {{
            for (int i = 0; i < count; i++) {
                add(in.readInt());
            }
        }};
        return new Stat.Response(filePartsIds);
    }

    @NotNull
    public Get.Response receiveGetResponse() throws IOException {
        final int count = in.readInt();
        final InputStream filePartContent = count == -1 ? null : new BoundedInputStream(in, count);
        return new Get.Response(filePartContent, count);
    }
}
