package ru.ifmo.torrent.client.seed;

import ru.ifmo.torrent.client.ClientConfig;
import ru.ifmo.torrent.client.storage.LocalFilesManager;
import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Seeder implements Runnable, AutoCloseable {

    private final LocalFilesManager filesManager;
    private final ServerSocket socket;
    private final ExecutorService pool = Executors.newFixedThreadPool(ClientConfig.SEED_THREADS_COUNT);


    public Seeder(short port, LocalFilesManager filesManager) throws IOException {
        this.filesManager = filesManager;
        socket = new ServerSocket(port);
    }

    public void run() {
        try (ServerSocket socket = this.socket) {
            while (true) {
                Socket leecherSocket = socket.accept();
                pool.submit(new LeechHandler(leecherSocket, filesManager));
            }
        } catch (IOException e) {
            if(!socket.isClosed()) {
                throw new IllegalStateException("cannot open seed socket", e);
            }
        }
    }

    @Override
    public void close() throws TorrentException {
        try {
            socket.close();
        } catch (IOException e) {
            throw new TorrentException("cannot close seed socket properly", e);
        } finally {
            pool.shutdown();
        }
    }
}
