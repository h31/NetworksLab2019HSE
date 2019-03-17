package ru.ifmo.torrent.messages.seed_peer;

import org.apache.commons.io.FileUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;
import ru.ifmo.torrent.messages.seed_peer.response.GetResponse;
import ru.ifmo.torrent.messages.seed_peer.response.StatResponse;
import ru.ifmo.torrent.messages.Response;

import java.io.*;
import java.nio.file.Files;
import java.util.Arrays;

import static org.assertj.core.api.Assertions.assertThat;


public class ResponseTests {

    @Rule
    public TemporaryFolder folder = new TemporaryFolder();

    private ByteArrayOutputStream baos = new ByteArrayOutputStream();
    private DataOutputStream out = new DataOutputStream(baos);

    @Test
    public void testGetResponse() throws IOException {
        File file = folder.newFile();
        FileUtils.writeStringToFile(file, "contentcontentcontentcontent");

        GetResponse sentResponse = new GetResponse(Files.newInputStream(file.toPath()), 21);
        GetResponse acceptedResponse = new GetResponse();
        sendAndAccept(sentResponse, acceptedResponse);

        assertThat(acceptedResponse.getContent()).isEqualTo(sentResponse.getContent());
    }

    @Test
    public void testStatResponse() throws IOException {
        StatResponse sentResponse = new StatResponse(Arrays.asList(0, 1, 2, 3, 4));
        StatResponse acceptedResponse = new StatResponse();
        sendAndAccept(sentResponse, acceptedResponse);

        assertThat(acceptedResponse.getAvailableParts()).containsExactlyElementsOf(sentResponse.getAvailableParts());
    }

    private void sendAndAccept(Response sentResponse, Response acceptedResponse) throws IOException {
        sentResponse.write(out);
        out.flush();
        DataInputStream in = new DataInputStream(new ByteArrayInputStream(baos.toByteArray()));
        acceptedResponse.read(in);
    }
}
