package org.itmo.torrent.network;

import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.network.messages.tracker.List;
import org.itmo.torrent.network.messages.tracker.Sources;
import org.itmo.torrent.network.messages.tracker.Update;
import org.itmo.torrent.network.messages.tracker.Upload;

import java.io.IOException;
import java.net.Socket;
import java.util.HashSet;
import java.util.Set;

public class Tracker2ClientConnection extends Connection {

    public Tracker2ClientConnection(@NotNull final Socket socket) throws IOException {
        super(socket);
    }

    public void sendListResponse(@NotNull final List.Response message) throws IOException {
        out.writeInt(message.getFileInfos().size());
        for (FileInfo fileInfo : message.getFileInfos()) {
            sendFileInfo(fileInfo);
        }
        out.flush();
    }

    public void sendUploadResponse(@NotNull final Upload.Response message) throws IOException {
        out.writeLong(message.getFileId());
        out.flush();
    }

    public void sendSourcesResponse(@NotNull final Sources.Response message) throws IOException {
        out.writeInt(message.getClientsAddresses().size());
        for (Address clientAddress : message.getClientsAddresses()) {
            sendAddress(clientAddress);
        }
        out.flush();
    }

    public void sendUpdateResponse(@NotNull final Update.Response message) throws IOException {
        out.writeBoolean(message.getStatus());
        out.flush();
    }

    @NotNull
    public List.Request receiveListRequest() {
        return new List.Request();
    }

    @NotNull
    public Upload.Request receiveUploadRequest() throws IOException {
        final String fileName = in.readUTF();
        final long fileSize = in.readLong();
        return new Upload.Request(fileName, fileSize);
    }

    @NotNull
    public Sources.Request receiveSourcesRequest() throws IOException {
        final int fileId = in.readInt();
        return new Sources.Request(fileId);
    }

    @NotNull
    public Update.Request receiveUpdateRequest() throws IOException {
        final short clientPort = in.readShort();
        final int count = in.readInt();
        final Set<Integer> filesIds = new HashSet<Integer>() {{
            for (int i = 0; i < count; i++) {
                add(in.readInt());
            }
        }};
        return new Update.Request(clientPort, filesIds);
    }

    private void sendFileInfo(@NotNull final FileInfo fileInfo) throws IOException {
        out.writeInt(fileInfo.getId());
        out.writeUTF(fileInfo.getName());
        out.writeLong(fileInfo.getSize());
    }

    private void sendAddress(@NotNull final Address address) throws IOException {
        out.write(address.getIp());
        out.writeShort(address.getPort());
    }
}
