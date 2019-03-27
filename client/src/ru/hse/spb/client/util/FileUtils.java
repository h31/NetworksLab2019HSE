package ru.hse.spb.client.util;

import org.jetbrains.annotations.NotNull;
import ru.hse.spb.protocol.Constants;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileUtils {
    public static @NotNull byte[] readFilePart(@NotNull FileInputStream input, int segmentNumber) throws IOException {
        int offset = calculateOffset(segmentNumber);
        byte[] buffer = new byte[Constants.FILE_PART_SIZE];
        input.read(buffer, offset, Constants.FILE_PART_SIZE);
        return buffer;
    }

    public static void writeFilePart(@NotNull FileOutputStream output, int segmentNumber, @NotNull byte[] buffer) throws IOException {
        int offset = calculateOffset(segmentNumber);
        output.write(buffer, offset, Constants.FILE_PART_SIZE);
    }

    private static int calculateOffset(int segmentNumber) {
        return segmentNumber * Constants.FILE_PART_SIZE;
    }

    private FileUtils() {
    }
}
