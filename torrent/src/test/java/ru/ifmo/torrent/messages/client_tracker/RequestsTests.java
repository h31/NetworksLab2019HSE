package ru.ifmo.torrent.messages.client_tracker;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;
import ru.ifmo.torrent.messages.client_tracker.requests.*;
import ru.ifmo.torrent.messages.Request;

import java.io.*;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class RequestsTests {

    @Rule
    public TemporaryFolder folder = new TemporaryFolder();

    private ByteArrayOutputStream baos = new ByteArrayOutputStream();
    private DataOutputStream out = new DataOutputStream(baos);

    @Test
    public void testListRequest() throws IOException, InstantiationException, IllegalAccessException {
        ListRequest sentRequest = new ListRequest();
        DataInputStream in = testSendAndAccept(sentRequest, Marker.LIST);
        ListRequest acceptedRequest = new ListRequest();

        acceptedRequest.read(in);
        int EOF = in.read();
        assertThat(EOF).isEqualTo(-1);
    }

    @Test
    public void testSourceRequest() throws IOException, InstantiationException, IllegalAccessException {
        SourcesRequest sentRequest = new SourcesRequest(0);
        SourcesRequest acceptedRequest = new SourcesRequest();
        DataInputStream in = testSendAndAccept(sentRequest, Marker.SOURCES);

        acceptedRequest.read(in);
        assertThat(sentRequest.getFileId()).isEqualTo(acceptedRequest.getFileId());
    }

    @Test
    public void testUpdateRequest() throws IOException, InstantiationException, IllegalAccessException {
        List<Integer> fileIDs = Arrays.asList(0, 1, 2);
        UpdateRequest sentRequest = new UpdateRequest((short) 1111, fileIDs);
        UpdateRequest acceptedRequest = new UpdateRequest();

        DataInputStream in = testSendAndAccept(sentRequest, Marker.UPDATE);
        acceptedRequest.read(in);
        assertThat(acceptedRequest.getClientPort()).isEqualTo(sentRequest.getClientPort());
        assertThat(acceptedRequest.getFileIds().size()).isEqualTo(sentRequest.getFileIds().size());
        for (int i = 0; i < acceptedRequest.getFileIds().size(); i++) {
            assertThat(acceptedRequest.getFileIds().get(i)).isEqualTo(sentRequest.getFileIds().get(i));
        }
    }

    @Test
    public void testUploadRequest() throws IOException, InstantiationException, IllegalAccessException {
        Path tmpFile = folder.newFile("file_name").toPath();

        UploadRequest sentRequest = new UploadRequest(tmpFile);
        UploadRequest acceptedRequest = new UploadRequest();

        DataInputStream in = testSendAndAccept(sentRequest, Marker.UPLOAD);
        acceptedRequest.read(in);
        assertThat(acceptedRequest.getFileName()).isEqualTo(sentRequest.getFileName());
        assertThat(acceptedRequest.getFileSize()).isEqualTo(sentRequest.getFileSize());
    }

    private DataInputStream testSendAndAccept(Request request, byte marker) throws IOException, IllegalAccessException, InstantiationException {
        request.write(out);
        out.flush();

        DataInputStream in = new DataInputStream(new ByteArrayInputStream(baos.toByteArray()));
        byte acceptedMarker = in.readByte();
        assertThat(acceptedMarker).isEqualTo(marker);
        return in;
    }
}
