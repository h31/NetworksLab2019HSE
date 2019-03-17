package ru.ifmo.torrent;

import org.apache.commons.io.FileUtils;
import org.junit.*;
import org.junit.rules.TemporaryFolder;
import ru.ifmo.torrent.client.Client;
import ru.ifmo.torrent.client.ClientConfig;
import ru.ifmo.torrent.tracker.Tracker;
import ru.ifmo.torrent.tracker.TrackerConfig;
import ru.ifmo.torrent.tracker.state.FileInfo;
import ru.ifmo.torrent.tracker.state.SeedInfo;
import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.net.InetAddress;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

import static junit.framework.TestCase.assertTrue;
import static org.assertj.core.api.Assertions.assertThat;

public class TorrentTest {
    private final int port1 = 1111;
    private final int port2 = 1199;
    private Tracker tracker;
    private Client client1;
    private Client client2;

    @Rule
    public TemporaryFolder folder = new TemporaryFolder();

    private Path folder1;
    private Path folder2;
    private Path downloadFolder1;
    private Path downloadFolder2;

    @Before
    public void runTorrent() throws IOException, TorrentException {
        folder1 = folder.newFolder("client1").toPath();
        folder2 = folder.newFolder("client2").toPath();
        downloadFolder1 = folder.newFolder("downloads1").toPath();
        downloadFolder2 = folder.newFolder("downloads2").toPath();

        tracker = new Tracker(TrackerConfig.TRACKER_PORT, folder1);
        tracker.run();

        client1 = new Client(InetAddress.getLocalHost(), (short) port1, folder1, downloadFolder1);
        client2 = new Client(InetAddress.getLocalHost(), (short) port2, folder2, downloadFolder2);
    }

    @After
    public void stopTorrent() throws TorrentException {
        tracker.close();
        client1.close();
        client2.close();
    }

    @Test
    public void testEmptyListRequest() throws IOException, TorrentException {
        List<FileInfo> availableFiles = client1.getAvailableFiles();
        assertThat(availableFiles).isEmpty();
    }

    @Test
    public void testUploadAndListRequest() throws IOException, TorrentException {
        Path file = createFile();
        addFileToTrackerAndCheckContains(file);
    }

    @Test
    public void testUploadFileAndSources() throws IOException, TorrentException, InterruptedException {
        Path file = createFile();
        FileInfo addedFile = addFileToTrackerAndCheckContains(file);

        Thread.sleep((ClientConfig.UPDATE_RATE_SEC + 1) * 1000);
        List<SeedInfo> sources = client1.getFileSources(addedFile.getId());

        SeedInfo seedAdded = new SeedInfo((short) port1, InetAddress.getByName("localhost"));
        assertThat(sources.size()).isEqualTo(1);
        assertThat(sources.get(0)).isEqualTo(seedAdded);
    }

    @Test
    public void testDownloadFile() throws IOException, TorrentException, InterruptedException {
        Path file = createFile();
        FileInfo addedFile = addFileToTrackerAndCheckContains(file);

        Thread.sleep((ClientConfig.UPDATE_RATE_SEC + 1) * 1000);
        boolean result = client2.downloadFile(addedFile.getId());
        Thread.sleep((ClientConfig.DOWNLOAD_RATE_SEC + 1) * 1000);

        Path downloadedFile = downloadFolder2.resolve(file.getFileName());
        assertTrue(result);
        assertTrue(Files.exists(downloadedFile));
        assertThat(Files.size(downloadedFile)).isEqualTo(Files.size(file));
        assertThat(FileUtils.readFileToString(downloadedFile.toFile()))
            .isEqualTo(FileUtils.readFileToString(file.toFile()));
    }

    private FileInfo addFileToTrackerAndCheckContains(Path file) throws IOException, TorrentException {
        int id = client1.uploadFile(file);
        List<FileInfo> availableFiles = client1.getAvailableFiles();
        assertThat(availableFiles.size()).isEqualTo(1);

        FileInfo addedFile = new FileInfo(id, file.getFileName().toString(), Files.size(file));
        assertThat(availableFiles.get(0)).isEqualTo(addedFile);

        return addedFile;
    }

    private Path createFile() throws IOException {
        Path file = folder1.resolve("fileName");
        Files.createFile(file);
        FileUtils.writeStringToFile(file.toFile(), "kek!");
        return file;
    }
}
