package ru.ifmo.torrent.tracker;

import ru.ifmo.torrent.tracker.state.TrackerState;
import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Path;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class Tracker implements AutoCloseable, Runnable {

    private final ExecutorService pool = Executors.newFixedThreadPool(TrackerConfig.THREADS_COUNT);
    private final TrackerState state;
    private final ServerSocket serverSocket;

    public Tracker(short port, Path metaDir) throws TorrentException, IOException {
        state = new TrackerState(metaDir.resolve(TrackerConfig.TRACKER_STATE_FILE));

        try {
            serverSocket = new ServerSocket(port);
        } catch (IOException e) {
            throw new TorrentException("cannot open server socket", e);
        }
    }

    @Override
    public void run() {
        pool.submit(() -> {
            try {
                while (!Thread.interrupted()) {
                    Socket client = serverSocket.accept();
                    pool.submit(new ClientHandler(client, state));
                }
            } catch (IOException e) {
                if (!serverSocket.isClosed()) {
                    throw new IllegalStateException("cannot close tracker socket", e);
                }
            }
        });
    }

    @Override
    public void close() throws TorrentException {
        try {
            serverSocket.close();
        } catch (IOException e) {
            throw new TorrentException("cannot close tracker properly", e);
        } finally {
            pool.shutdown();
            state.storeToFile();
        }
    }

}
