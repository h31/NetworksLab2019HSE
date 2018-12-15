package itmo2018.se;

import java.io.*;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.stream.Collectors;

public class MetaDataManager {
    private File file;
    private int filesCount;

    public MetaDataManager(File file) throws IOException {
        this.file = file;
        if (!file.exists() || file.isDirectory()) {
            throw new FileNotFoundException();
        }
        this.filesCount = (int) Files.lines(file.toPath()).count();
    }

    public void addReadyNote(int id, String name, long size) throws IOException {
        try (OutputStream writer = new FileOutputStream(file, true)) {
            writer.write((id + "\t" + name + "\t" + size + "\t" + -1 + "\n").getBytes());
        }
        filesCount++;
    }

    public void addNote(int id, String name, long size) throws IOException {
        try (OutputStream writer = new FileOutputStream(file, true)) {
            writer.write((id + "\t" + name + "\t" + size + "\n").getBytes());
        }
        filesCount++;
    }

    public void addNote(int id, String name, long size, Set<Integer> parts) throws IOException {
        long partSize = 1024L * 1024L * 5;
        int partsCount = (int) ((size - 1) / partSize + 1);
        if (parts.size() == partsCount) {
            addReadyNote(id, name, size);
        } else {
            try (OutputStream writer = new FileOutputStream(file, true)) {
                writer.write((id + "\t" + name + "\t" + size).getBytes());
                for (int part : parts) {
                    writer.write(("\t" + part).getBytes());
                }
                writer.write("\n".getBytes());
            }
        }
        filesCount++;
    }

    public void addPart(int id, int part) throws IOException {
        List<String> lines = Files.lines(file.toPath()).collect(Collectors.toList());
        try (OutputStream writer = new FileOutputStream(file, false)) {
            for (String line : lines) {
                if (line.startsWith(id + "\t")) {
                    String[] data = line.split("\t");
                    if (data.length == 3) {
                        writer.write((line + "\t" + part + "\n").getBytes());
                    } else if (!data[3].equals("-1")) {
                        writer.write((line + "," + part + "\n").getBytes());
                    }
                } else {
                    writer.write((line + "\n").getBytes());
                }
            }
        }
    }

    public void deleteNote(int id) throws IOException {
        List<String> lines = Files.lines(file.toPath()).collect(Collectors.toList());
        try (OutputStream writer = new FileOutputStream(file, false)) {
            for (String line : lines) {
                if (!line.startsWith(id + "\t")) {
                    writer.write((line + "\n").getBytes());
                } else {
                    filesCount--;
                }
            }
        }
    }

    public MetaDataNote getNote(int id) throws IOException {
        Optional<String[]> optionalfileDescription = Files.lines(file.toPath())
                .filter(it -> it.startsWith(id + "\t"))
                .map(it -> it.split("\t"))
                .findFirst();
        if (!optionalfileDescription.isPresent()) {
            return null;
        }
        String[] fileDescription = optionalfileDescription.get();
        if (fileDescription[3].equals("-1")) {
            return new MetaDataNote(Integer.parseInt(fileDescription[0]),
                    fileDescription[1], Long.parseLong(fileDescription[2]));
        }
        Set<Integer> parts = Arrays.stream(fileDescription[3].split(","))
                .map(Integer::parseInt).collect(Collectors.toSet());
        MetaDataNote result = new MetaDataNote(Integer.parseInt(fileDescription[0]),
                fileDescription[1], Long.parseLong(fileDescription[2]), parts);
        if (result.isFinish()) {
            finishCollectParts(id);
        }
        return result;
    }

    public int filesCount() {
        return filesCount;
    }

    public Set<Integer> idSet() throws IOException {
        return Files.lines(file.toPath()).map(it -> Integer.parseInt(it.split("\t")[0]))
                .collect(Collectors.toSet());
    }

    public void finishCollectParts(int id) throws IOException {
        List<String> lines = Files.lines(file.toPath()).collect(Collectors.toList());
        try (OutputStream writer = new FileOutputStream(file, false)) {
            for (String line : lines) {
                if (line.startsWith(id + "\t")) {
                    String[] data = line.split("\t");
                    writer.write((data[0] + "\t" + data[1] + "\t" + data[2] + "\t" + -1 + "\n").getBytes());
                } else {
                    writer.write((line + "\n").getBytes());
                }
            }
        }
    }

//    private void isExist(int id) throws IOException {
//        if (Files.lines(file.toPath())
//                .anyMatch(it -> it.startsWith(id + "\t"))) {
//            throw new IOException("file with " + id + " id already exist");
//        }
//    }
}
