package org.itmo.torrent;

import org.jetbrains.annotations.NotNull;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public abstract class AbstractServer implements Server, AutoCloseable {
    private final short port;
    @NotNull
    private final ExecutorService service = Executors.newCachedThreadPool();
    @NotNull
    private final ServerSocket serverSocket;

    public AbstractServer(final short port) throws IOException {
        this.port = port;
        serverSocket = new ServerSocket(port);
        service.submit(new ConnectionsListener());
    }

    public short getPort() {
        return port;
    }

    public void close() throws IOException {
        serverSocket.close();
        service.shutdown();
    }

    protected abstract Runnable getConnectionHandler(@NotNull final Socket clientSocket);

    private class ConnectionsListener implements Runnable {
        @Override
        public void run() {
            while (!Thread.interrupted()) {
                try {
                    final Socket clientSocket = serverSocket.accept();
                    service.submit(getConnectionHandler(clientSocket));
                } catch (IOException ignore) {
                }
            }
        }
    }
}
