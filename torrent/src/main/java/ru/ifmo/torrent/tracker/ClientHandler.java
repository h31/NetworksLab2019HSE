package ru.ifmo.torrent.tracker;

import ru.ifmo.torrent.messages.client_tracker.Marker;
import ru.ifmo.torrent.messages.client_tracker.requests.*;
import ru.ifmo.torrent.messages.client_tracker.response.*;
import ru.ifmo.torrent.messages.Response;
import ru.ifmo.torrent.tracker.state.FileInfo;
import ru.ifmo.torrent.tracker.state.SeedInfo;
import ru.ifmo.torrent.tracker.state.TrackerState;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.List;
import java.util.Set;
import java.util.stream.Collectors;

public class ClientHandler implements Runnable {

    private final Socket client;
    private final TrackerState trackerState;

    ClientHandler(Socket client, TrackerState trackerState) {
        this.client = client;
        this.trackerState = trackerState;
    }

    @Override
    public void run() {
        try (Socket clientSocket = client) {
            DataOutputStream out = new DataOutputStream(clientSocket.getOutputStream());
            DataInputStream in = new DataInputStream(clientSocket.getInputStream());

            Response response = null;
            int marker;
            while ((marker = in.read()) != -1) {
                switch (marker) {
                    case Marker.LIST:
                        response = new ListResponse(trackerState.getAvailableFiles());
                        break;
                    case Marker.SOURCES: {
                        SourcesRequest request = (SourcesRequest) SourcesRequest.readFromDataInputStream(in, SourcesRequest.class);
                        int fileId = request.getFileId();
                        response = new SourcesResponse(fileId, trackerState.getSources(fileId));
                        break;
                    }
                    case Marker.UPDATE: {
                        UpdateRequest request = (UpdateRequest) UpdateRequest.readFromDataInputStream(in, UpdateRequest.class);
                        InetAddress address = InetAddress.getByName(clientSocket.getInetAddress().getHostAddress());
                        SeedInfo newSeed = new SeedInfo(request.getClientPort(), address);
                        boolean success = update(request.getFileIds(), newSeed);
                        response = new UpdateResponse(success);
                        break;
                    }
                    case Marker.UPLOAD: {
                        UploadRequest request = (UploadRequest) UploadRequest.readFromDataInputStream(in, UploadRequest.class);
                        int fileId = trackerState.addFile(request.getFileName(), request.getFileSize());
                        response = new UploadResponse(fileId);
                        break;
                    }
                    default:
                        break;
                }
                if (response != null) {
                    response.write(out);
                    out.flush();
                }
            }
        } catch (Exception e) {
            throw new IllegalStateException("error on tracker acquired", e);
        }
    }

    private boolean update(List<Integer> fileIds, SeedInfo newSeed) {
        Set<Integer> allFiles = trackerState.getAvailableFiles().stream()
            .map(FileInfo::getId)
            .collect(Collectors.toSet());

        if (!allFiles.containsAll(fileIds)) return false;

        fileIds.forEach(id -> trackerState.addNewSeedIfAbsent(id, newSeed));
        return true;
    }
}
