package ru.ifmo.torrent.client.seed;

import ru.ifmo.torrent.client.storage.LocalFileReference;
import ru.ifmo.torrent.client.storage.LocalFilesManager;
import ru.ifmo.torrent.messages.Request;
import ru.ifmo.torrent.messages.seed_peer.Marker;
import ru.ifmo.torrent.messages.seed_peer.requests.*;
import ru.ifmo.torrent.messages.seed_peer.response.*;
import ru.ifmo.torrent.messages.Response;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.util.List;

class LeechHandler implements Runnable {

    private final Socket leechSocket;
    private final LocalFilesManager filesManager;

    LeechHandler(Socket leecher, LocalFilesManager localFilesManager) {
        this.leechSocket = leecher;
        this.filesManager = localFilesManager;
    }

    @Override
    public void run() {
        try (Socket socket = leechSocket) {
            DataInputStream in = new DataInputStream(socket.getInputStream());
            DataOutputStream out = new DataOutputStream(socket.getOutputStream());

            Response response = null;
            int marker = in.readByte();
            switch (marker) {
                case Marker.GET: {
                    GetRequest request = (GetRequest) Request.readFromDataInputStream(in, GetRequest.class);
                    InputStream is = getPartForDownloading(request.getFileId(), request.getPart());
                    LocalFileReference reference = filesManager.getFileReference(request.getFileId());
                    response = new GetResponse(is, reference.getBlockSizeForPart(request.getPart()));
                    is.close();
                    break;
                }
                case Marker.STAT: {
                    StatRequest request = (StatRequest) StatRequest.readFromDataInputStream(in, StatRequest.class);
                    response = new StatResponse(getParts(request.getFileId()));
                    break;
                }
                default:
                    break;
            }

            if (response != null) {
                response.write(out);
                out.flush();
            }

        } catch (IOException | IllegalAccessException | InstantiationException e) {
            System.err.printf("error while service leech %s %d%n",  leechSocket.getInetAddress(), leechSocket.getPort());
        }

    }

    private InputStream getPartForDownloading(int fileId, int filePart) throws IOException {
        return filesManager.getPartsManager().getForReading(fileId, filePart);
    }

    private List<Integer> getParts(int fileId) {
        LocalFileReference file = filesManager.getFileReference(fileId);
        return file.getReadyParts();
    }
}