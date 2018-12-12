package itmo2018.se;

import java.util.List;

public interface FileManager extends Iterable<FileInfo> {
    FileInfo getFile(int id);

    int registerFile(String name, long size, ClientInfo owner);

    void setFile(int id, String name, long size);

    void addOwner(int fileId, ClientInfo client);

    int filesNumber();
}
