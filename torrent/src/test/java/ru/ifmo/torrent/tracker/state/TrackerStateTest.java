package ru.ifmo.torrent.tracker.state;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;
import ru.ifmo.torrent.tracker.TrackerConfig;
import ru.ifmo.torrent.util.TorrentException;

import java.io.IOException;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

import static junit.framework.TestCase.assertTrue;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;

public class TrackerStateTest {

    private final List<FileInfo> files = Arrays.asList(
        new FileInfo(1, "file_1", 1),
        new FileInfo(2, "file_2", 100),
        new FileInfo(3, "file_3", 17)
    );

    @Rule
    public TemporaryFolder folder = new TemporaryFolder();

    private Path createMetaFile() throws IOException {
        return folder.newFile(TrackerConfig.TRACKER_STATE_FILE).toPath();
    }

    @Test
    public void testStoringAndRestoringState() throws IOException, TorrentException {
        Path file = createMetaFile();
        TrackerState storedState = new TrackerState(file);
        files.forEach(f -> storedState.addFile(f.getName(), f.getSize()));
        storedState.storeToFile();

        TrackerState restoredState = new TrackerState(file);
        List<FileInfo> restoredFiles = restoredState.getAvailableFiles();

        assertThat(files.size()).isEqualTo(restoredFiles.size());
        for (int i = 0; i < files.size(); i++) {
            assertThat(restoredFiles.get(i).getId()).isEqualTo(files.get(i).getId());
            assertThat(restoredFiles.get(i).getName()).isEqualTo(files.get(i).getName());
            assertThat(restoredFiles.get(i).getSize()).isEqualTo(files.get(i).getSize());
        }
    }

    @Test
    public void addAndContainsFileTest() throws IOException, TorrentException {
        Path file = createMetaFile();
        TrackerState state = new TrackerState(file);
        assertTrue(state.getAvailableFiles().isEmpty());

        String fileName = "kek";
        long fileSize = 17;
        int ID = state.addFile(fileName, fileSize);
        assertThat(state.getAvailableFiles().size()).isEqualTo(1);

        FileInfo addedFile = state.getAvailableFiles().get(0);
        assertThat(addedFile.getId()).isEqualTo(ID);
        assertThat(addedFile.getName()).isEqualTo(fileName);
        assertThat(addedFile.getSize()).isEqualTo(fileSize);
    }

}
