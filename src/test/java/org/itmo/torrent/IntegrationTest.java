package org.itmo.torrent;

import org.itmo.torrent.tracker.TorrentTracker;
import org.junit.*;
import org.junit.rules.TemporaryFolder;
import org.itmo.torrent.client.TorrentClient;
import org.itmo.torrent.filesystem.FileInfo;
import org.itmo.torrent.network.Address;
import org.itmo.torrent.network.messages.tracker.Update;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.*;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

import static org.junit.Assert.assertEquals;

public class IntegrationTest {
    private static final short CLIENT1_PORT = 1111;
    private static final short CLIENT2_PORT = 2222;
    private static final short CLIENT3_PORT = 3333;

    @Rule
    public TemporaryFolder trackerFolder = new TemporaryFolder();
    @Rule
    public TemporaryFolder client1Folder = new TemporaryFolder();
    @Rule
    public TemporaryFolder client2Folder = new TemporaryFolder();
    @Rule
    public TemporaryFolder client3Folder = new TemporaryFolder();

    private TorrentTracker tracker;
    private TorrentClient client1;
    private TorrentClient client2;
    private TorrentClient client3;

    @Before
    public void setUp() throws Exception {
        tracker = new TorrentTracker(trackerFolder.getRoot().toPath());
        client1 = new TorrentClient(CLIENT1_PORT, client1Folder.getRoot().toPath());
        client2 = new TorrentClient(CLIENT2_PORT, client2Folder.getRoot().toPath());
        client3 = new TorrentClient(CLIENT3_PORT, client3Folder.getRoot().toPath());
    }

    @After
    public void tearDown() throws Exception {
        if (tracker != null) {
            tracker.close();
        }
        if (client1 != null) {
            client1.close();
        }
        if (client2 != null) {
            client2.close();
        }
        if (client3 != null) {
            client3.close();
        }

        Thread.sleep(50);
    }

    @Test
    public void testListEmpty() throws IOException {
        final List<FileInfo> expected = Collections.emptyList();
        final List<FileInfo> actual = client1.executeList();
        assertEquals(expected, actual);
    }

    @Test
    public void testUpload1() throws IOException {
        final String fileName = "dummy.txt";
        final File file = client1Folder.newFile(fileName);
        client1.executeUpload(file);
        final List<FileInfo> expected = new ArrayList<FileInfo>() {{
            add(new FileInfo(0, fileName, 0));
        }};
        final List<FileInfo> actual = client1.executeList();
        assertEquals(expected, actual);
    }

    @Test
    public void testUpload2() throws IOException {
        final String file1Name = "dummy1.txt";
        final String file2Name = "dummy2.txt";
        final File file1 = client1Folder.newFile(file1Name);
        final File file2 = client2Folder.newFile(file2Name);
        client1.executeUpload(file1);
        client2.executeUpload(file2);
        final Set<FileInfo> expected = new HashSet<FileInfo>() {{
            add(new FileInfo(0, file1Name, 0));
            add(new FileInfo(1, file2Name, 0));
        }};
        final Set<FileInfo> actual1 = new HashSet<>(client1.executeList());
        final Set<FileInfo> actual2 = new HashSet<>(client2.executeList());
        assertEquals(expected, actual1);
        assertEquals(expected, actual2);
    }

    @Test
    public void testUploadBigFile() throws IOException {
        final String fileName = "dummy1.txt";
        final int newLength = 100000;
        uploadBigFile(0);
        final List<FileInfo> expected = new ArrayList<FileInfo>() {{
            add(new FileInfo(0, fileName, newLength));
        }};
        final List<FileInfo> actual = client1.executeList();
        assertEquals(expected, actual);
    }

    @Test
    public void testUploadSameFile() throws IOException {
        final String fileName = "dummy.txt";
        final File file = client1Folder.newFile(fileName);
        client1.executeUpload(file);
        client1.executeUpload(file);
        final Set<FileInfo> expected = new HashSet<FileInfo>() {{
            add(new FileInfo(0, fileName, 0));
            add(new FileInfo(1, fileName, 0));
        }};
        final Set<FileInfo> actual = new HashSet<>(client1.executeList());
        assertEquals(expected, actual);
    }

    //    @Ignore // too long to wait
    @Test
    public void testUpdateSources() throws IOException, InterruptedException {
        final String fileName = "dummy.txt";
        final File file = client1Folder.newFile(fileName);
        client1.executeUpload(file);

        Thread.sleep(Update.UPDATE_PERIOD * 2);

        final Set<Address> expected = new HashSet<Address>() {{
            add(new Address(new byte[]{127, 0, 0, 1}, CLIENT1_PORT));
        }};
        final Set<Address> actual = client2.executeSources(0);
        assertEquals(expected, actual);
    }

    //    @Ignore // too long to wait
    @Test
    public void testUpdateExpire() throws IOException, InterruptedException {
        final String fileName = "dummy.txt";
        final File file = client1Folder.newFile(fileName);
        client1.executeUpload(file);

        Thread.sleep(Update.UPDATE_PERIOD * 2);

        client1.close();

        Thread.sleep(Update.UPDATE_PERIOD * 2);

        final Set<Address> expected = Collections.emptySet();
        final Set<Address> actual = client2.executeSources(0);
        assertEquals(expected, actual);
    }

    @Test
    public void testTrackerPersistentState() throws IOException, ClassNotFoundException {
        final String fileName = "dummy.txt";
        final File file = client1Folder.newFile(fileName);
        client1.executeUpload(file);

        tracker.close();
        tracker = new TorrentTracker(trackerFolder.getRoot().toPath());

        final List<FileInfo> expected = new ArrayList<FileInfo>() {{
            add(new FileInfo(0, fileName, 0));
        }};
        final List<FileInfo> actual = client2.executeList();
        assertEquals(expected, actual);
    }

    @Ignore // too long to wait
    @Test
    public void testClientPersistentState()
            throws IOException, InterruptedException, ClassNotFoundException {
        final String fileName = "dummy.txt";
        final File file = client1Folder.newFile(fileName);
        client1.executeUpload(file);
        client1.close();
        client1 = new TorrentClient(CLIENT1_PORT, client1Folder.getRoot().toPath());

        Thread.sleep(Update.UPDATE_PERIOD * 5);

        final Set<Address> expected = new HashSet<Address>() {{
            add(new Address(new byte[]{127, 0, 0, 1}, CLIENT1_PORT));
        }};
        final Set<Address> actual = client2.executeSources(0);
        assertEquals(expected, actual);
    }

    @Test
    public void testStat() throws IOException {
        final int newLength = 100000;
        final int fileId = uploadBigFile(0);
        final Set<Integer> expected = IntStream.range(0, newLength / (int) FileInfo.FILE_CHUNK_SIZE + 1)
                .boxed()
                .collect(Collectors.toSet());
        final Address address = new Address(new byte[]{127, 0, 0, 1}, CLIENT1_PORT);
        final Set<Integer> actual = client2.executeStat(address, fileId);
        assertEquals(expected, actual);
    }

    @Test
    public void testDownload() throws IOException, InterruptedException {
        final int newLength = 100000;
        final int fileId = uploadBigFile(0);
        Thread.sleep(Update.UPDATE_PERIOD * 2);
        final Set<Integer> expected = IntStream.range(0, newLength / (int) FileInfo.FILE_CHUNK_SIZE + 1)
                .boxed()
                .collect(Collectors.toSet());
        client2.executeDownload(fileId);
        Thread.sleep(100);
        Thread.sleep(Update.UPDATE_PERIOD * 2);
        final Address address = new Address(new byte[]{127, 0, 0, 1}, CLIENT2_PORT);
        final Set<Integer> actual = client3.executeStat(address, fileId);
        assertEquals(expected, actual);
    }

    //    @Ignore // too long to wait
    @Test
    public void testDownloadParallel() throws InterruptedException, IOException {
        final int newLength = 100000;
        final int fileId1 = uploadBigFile(0);
        final int fileId2 = uploadBigFile(1);
        final int fileId3 = uploadBigFile(2);
        Thread.sleep(Update.UPDATE_PERIOD * 2);
        final Set<Integer> expected = IntStream.range(0, newLength / (int) FileInfo.FILE_CHUNK_SIZE + 1)
                .boxed()
                .collect(Collectors.toSet());
        client1.executeDownload(fileId2);
        client1.executeDownload(fileId3);
        client2.executeDownload(fileId1);
        client2.executeDownload(fileId3);
        client3.executeDownload(fileId1);
        client3.executeDownload(fileId2);
        Thread.sleep(100);
        Thread.sleep(Update.UPDATE_PERIOD * 2);
        final Address address1 = new Address(new byte[]{127, 0, 0, 1}, CLIENT1_PORT);
        final Address address2 = new Address(new byte[]{127, 0, 0, 1}, CLIENT2_PORT);
        final Address address3 = new Address(new byte[]{127, 0, 0, 1}, CLIENT3_PORT);
        final Set<Integer> actual1 = client1.executeStat(address2, fileId1);
        assertEquals(expected, actual1);
        final Set<Integer> actual2 = client1.executeStat(address2, fileId3);
        assertEquals(expected, actual2);
        final Set<Integer> actual3 = client2.executeStat(address3, fileId1);
        assertEquals(expected, actual3);
        final Set<Integer> actual4 = client2.executeStat(address3, fileId2);
        assertEquals(expected, actual4);
        final Set<Integer> actual5 = client3.executeStat(address1, fileId2);
        assertEquals(expected, actual5);
        final Set<Integer> actual6 = client3.executeStat(address1, fileId3);
        assertEquals(expected, actual6);
    }

    private int uploadBigFile(final int i) throws IOException {
        final String[] fileName = {"dummy1.txt", "dummy2.txt", "dummy3.txt"};
        final TemporaryFolder[] clientFolder = {client1Folder, client2Folder, client3Folder};
        final File file = clientFolder[i].newFile(fileName[i]);
        final int newLength = 100000;
        try (RandomAccessFile randomAccessFile = new RandomAccessFile(file, "rw")) {
            randomAccessFile.setLength(newLength);
        }
        final TorrentClient[] client = {client1, client2, client3};
        return client[i].executeUpload(file);
    }
}
