package org.itmo.torrent.network;

import org.jetbrains.annotations.NotNull;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.network.messages.tracker.List;
import org.itmo.torrent.network.messages.tracker.Sources;
import org.itmo.torrent.network.messages.tracker.Update;
import org.itmo.torrent.network.messages.tracker.Upload;

import java.io.IOException;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

public class Client2TrackerConnection extends Connection {

    public Client2TrackerConnection(@NotNull final Socket socket) throws IOException {
        super(socket);
    }

    public void sendListRequest(@NotNull final List.Request message) throws IOException {
        out.writeByte(List.Request.ID);
        out.flush();
    }

    public void sendUploadRequest(@NotNull final Upload.Request message) throws IOException {
        out.writeByte(Upload.ID);
        out.writeUTF(message.getFileName());
        out.writeLong(message.getFileSize());
        out.flush();
    }

    public void sendSourcesRequest(@NotNull final Sources.Request message) throws IOException {
        out.writeByte(Sources.ID);
        out.writeInt(message.getFileId());
        out.flush();
    }

    public void sendUpdateRequest(@NotNull final Update.Request message) throws IOException {
        out.writeByte(Update.ID);
        out.writeShort(message.getClientPort());
        out.writeInt(message.getFilesIds().size());
        for (int fileId : message.getFilesIds()) {
            out.writeInt(fileId);
        }
        out.flush();
    }

    @NotNull
    public List.Response receiveListResponse() throws IOException {
        final int count = in.readInt();
        final java.util.List<FileInfo> fileInfos = new ArrayList<FileInfo>() {{
            for (int i = 0; i < count; i++) {
                add(receiveFileInfo());
            }
        }};
        return new List.Response(fileInfos);
    }

    @NotNull
    public Upload.Response receiveUploadResponse() throws IOException {
        final int fileId = in.readInt();
        return new Upload.Response(fileId);
    }

    @NotNull
    public Sources.Response receiveSourcesResponse() throws IOException {
        final int count = in.readInt();
        final Set<Address> clientsAddresses = new HashSet<Address>() {{
            for (int i = 0; i < count; i++) {
                add(receiveAddress());
            }
        }};
        return new Sources.Response(clientsAddresses);
    }

    @NotNull
    public Update.Response receiveUpdateResponse() throws IOException {
        final boolean status = in.readBoolean();
        return new Update.Response(status);
    }

    @NotNull
    private FileInfo receiveFileInfo() throws IOException {
        final int id = in.readInt();
        final String name = in.readUTF();
        final long size = in.readLong();
        return new FileInfo(id, name, size);
    }

    @NotNull
    private Address receiveAddress() throws IOException {
        final byte[] ip = new byte[4];
        in.readFully(ip);
        final short port = in.readShort();
        return new Address(ip, port);
    }
}
