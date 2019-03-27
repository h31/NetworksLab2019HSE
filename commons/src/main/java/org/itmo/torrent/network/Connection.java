package org.itmo.torrent.network;

import org.jetbrains.annotations.NotNull;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

public abstract class Connection implements AutoCloseable {
    @NotNull
    protected final DataInputStream in;
    @NotNull
    protected final DataOutputStream out;
    @NotNull
    private final Socket socket;

    public Connection(@NotNull final Socket socket) throws IOException {
        this.socket = socket;
        in = new DataInputStream(socket.getInputStream());
        out = new DataOutputStream(socket.getOutputStream());
    }

    public byte receiveRequestType() throws IOException {
        return in.readByte();
    }

    @Override
    public void close() throws IOException {
//        socket.close(); //  TODO: really?
    }
}
