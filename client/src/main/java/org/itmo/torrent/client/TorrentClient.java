package org.itmo.torrent.client;

import org.itmo.torrent.Constants;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.filesystem.FileSeedInfo;
import org.itmo.torrent.filesystem.FileSystemManager;
import org.itmo.torrent.network.Address;
import org.itmo.torrent.network.Client2PeerConnection;
import org.itmo.torrent.network.Client2TrackerConnection;
import org.itmo.torrent.network.messages.client.Get;
import org.itmo.torrent.network.messages.client.Stat;
import org.itmo.torrent.network.messages.tracker.List;
import org.itmo.torrent.network.messages.tracker.Sources;
import org.itmo.torrent.network.messages.tracker.Update;
import org.itmo.torrent.network.messages.tracker.Upload;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.nio.file.Path;
import java.util.Set;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class TorrentClient implements Client, AutoCloseable {
    @NotNull
    private final TorrentClientState state = new TorrentClientState();
    @NotNull
    private final Seeder seeder;
    @NotNull
    private final ScheduledExecutorService updater = Executors.newScheduledThreadPool(1);
    @NotNull
    private final Downloader downloader = new Downloader(this);

    public TorrentClient(final short port, @NotNull final Path rootFolderPath)
            throws IOException, ClassNotFoundException {
        FileSystemManager.getInstance().setRootFolderPath(rootFolderPath);
        state.restore(rootFolderPath);
        seeder = new Seeder(state, port);
        final Runnable updateAction = () -> {
            try {
                executeUpdate();
            } catch (IOException ignored) {
            }
        };
        updater.scheduleAtFixedRate(updateAction, 0, Update.UPDATE_PERIOD, TimeUnit.MILLISECONDS);
    }

    @Override
    public void close() throws IOException {
        downloader.close();
        seeder.close();
        updater.shutdown();
        final FileSystemManager manager = FileSystemManager.getInstance();
        state.save(manager.getRootFolderPath());
    }

    @NotNull
    @Override
    public java.util.List<FileInfo> executeList() throws IOException {
        try (Client2TrackerConnection connection = connectToTracker()) {
            final List.Request request = new List.Request();
            connection.sendListRequest(request);
            final List.Response response = connection.receiveListResponse();
            return response.getFileInfos();
        }
    }

    @Override
    public int executeUpload(@NotNull final File file) throws IOException {
        if (!file.isFile()) {
            throw new IllegalArgumentException("File doesn't exist, or it's not a normal file");
        }

        try (Client2TrackerConnection connection = connectToTracker()) {
            final String fileName = file.getName();
            final long fileSize = file.length();
            final Upload.Request request = new Upload.Request(fileName, fileSize);
            connection.sendUploadRequest(request);
            final Upload.Response response = connection.receiveUploadResponse();
            final int fileId = response.getFileId();
            final FileSeedInfo fileInfo = new FileSeedInfo(fileId, fileName, fileSize, file);
            state.addFileInfo(fileInfo);
            return fileId;
        }
    }

    @NotNull
    @Override
    public Set<Address> executeSources(final int fileId) throws IOException {
        try (Client2TrackerConnection connection = connectToTracker()) {
            final Sources.Request request = new Sources.Request(fileId);
            connection.sendSourcesRequest(request);
            final Sources.Response response = connection.receiveSourcesResponse();
            return response.getClientsAddresses();
        }
    }

    @Override
    public boolean executeUpdate() throws IOException {
        try (Client2TrackerConnection connection = connectToTracker()) {
            final Update.Request request = new Update.Request(seeder.getPort(), state.getFilesIds());
            connection.sendUpdateRequest(request);
            final Update.Response response = connection.receiveUpdateResponse();
            return response.getStatus();
        }
    }

    @Override
    public void executeDownload(final int fileId) {
        downloader.download(fileId);
    }

    @NotNull
    @Override
    public Set<Integer> executeStat(@NotNull final Address address,
                                    final int fileId) throws IOException {
        try (Client2PeerConnection connection = connectToPeer(address)) {
            final Stat.Request request = new Stat.Request(fileId);
            connection.sendStatRequest(request);
            final Stat.Response response = connection.receiveStatResponse();
            return response.getFileChunksIds();
        }
    }

    @Nullable
    @Override
    public InputStream executeGet(@NotNull final Address address,
                                  final int fileId, final int fileChunkId) throws IOException {
        try (Client2PeerConnection connection = connectToPeer(address)) {
            final Get.Request request = new Get.Request(fileId, fileChunkId);
            connection.sendGetRequest(request);
            final Get.Response response = connection.receiveGetResponse();
            return response.getCount() == -1 ? null : response.getFileChunkContent();
        }
    }

    @NotNull
    TorrentClientState getState() {
        return state;
    }

    @NotNull
    private Client2TrackerConnection connectToTracker() throws IOException {
        final Socket socket = new Socket(Constants.TRACKER_HOST, Constants.TRACKER_PORT);
        return new Client2TrackerConnection(socket);
    }

    @NotNull
    private Client2PeerConnection connectToPeer(@NotNull final Address address) throws IOException {
        final Socket socket = new Socket(address.getHost(), address.getPort());
        return new Client2PeerConnection(socket);
    }
}
