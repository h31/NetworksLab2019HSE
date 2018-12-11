package itmo2018.se;

import java.util.List;

public interface FileManager extends Iterable<FileInfo> {
    FileInfo getFile(int id);

    void registerFile(String name);

    void addOwner(int fileId, ClientInfo client, List<Integer> parts);
}
