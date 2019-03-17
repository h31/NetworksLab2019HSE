package ru.ifmo.torrent.messages.seed_peer;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;
import ru.ifmo.torrent.messages.seed_peer.requests.GetRequest;
import ru.ifmo.torrent.messages.seed_peer.requests.StatRequest;
import ru.ifmo.torrent.messages.Request;

import java.io.*;

import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class RequestTests {
    @Rule
    public TemporaryFolder folder = new TemporaryFolder();

    private ByteArrayOutputStream baos = new ByteArrayOutputStream();
    private DataOutputStream out = new DataOutputStream(baos);

    @Test
    public void testGetRequest() throws IOException {
        GetRequest sentRequest = new GetRequest(17, 2);
        DataInputStream in = testSendAndAccept(sentRequest, Marker.GET);
        GetRequest acceptedRequest = new GetRequest();

        acceptedRequest.read(in);
        assertThat(acceptedRequest.getFileId()).isEqualTo(sentRequest.getFileId());
        assertThat(acceptedRequest.getPart()).isEqualTo(sentRequest.getPart());
    }

    @Test
    public void testStatRequest() throws IOException {
        StatRequest sentRequest = new StatRequest(17);
        StatRequest acceptedRequest = new StatRequest();
        DataInputStream in = testSendAndAccept(sentRequest, Marker.STAT);

        acceptedRequest.read(in);
        assertThat(sentRequest.getFileId()).isEqualTo(acceptedRequest.getFileId());
    }

    private DataInputStream testSendAndAccept(Request request, byte marker) throws IOException {
        request.write(out);
        out.flush();

        DataInputStream in = new DataInputStream(new ByteArrayInputStream(baos.toByteArray()));
        byte acceptedMarker = in.readByte();
        assertThat(acceptedMarker).isEqualTo(marker);
        return in;
    }
}
