package ru.ifmo.torrent.client.storage;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;
import ru.ifmo.torrent.client.storage.LocalFileReference;
import ru.ifmo.torrent.client.storage.PartsManager;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

import static org.assertj.core.api.Assertions.assertThat;

public class PartsManagerTest {

    @Rule
    public TemporaryFolder folder = new TemporaryFolder();

    @Test
    public void testStoreSplittedAndThenReading() throws IOException {
        Path file = folder.newFile().toPath();
        String content = "content";
        FileUtils.writeStringToFile(file.toFile(), content);
        PartsManager partsManager = new PartsManager(folder.getRoot().toPath());

        int id = 0;
        LocalFileReference reference = LocalFileReference.createEmpty(file.getFileName().toString(), 0, Files.size(file), 1);
        partsManager.storeSplitted(reference, file);
        String storedContent = IOUtils.toString(partsManager.getForReading(id,0));
        assertThat(storedContent).isEqualTo(content);
    }

    @Test
    public void testMergeParts() throws IOException {
        Path fileDir = folder.newFolder("0").toPath();
        PartsManager partsManager = new PartsManager(folder.getRoot().toPath());
        List<Path> files = Arrays.asList(
            Files.createFile(fileDir.resolve("0")),
            Files.createFile(fileDir.resolve("1")),
            Files.createFile(fileDir.resolve("2"))
        );
        List<String> contents = Arrays.asList("content1", "content2", "content3");

        for (int i = 0; i < contents.size(); i++) {
            FileUtils.writeStringToFile(files.get(i).toFile(), contents.get(i));
        }

        Path mergedFile = folder.newFile().toPath();
        partsManager.mergeSplitted(0, 24, mergedFile);

        String storedContent = FileUtils.readFileToString(mergedFile.toFile());
        assertThat(storedContent).isEqualTo("content1content2content3");
    }
}
