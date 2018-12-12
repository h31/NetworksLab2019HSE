package itmo2018.se;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class ListFileManager implements FileManager {
    private List<FileInfo> files = new ArrayList<>();

    @Override
    public FileInfo getFile(int id) {
        FileInfo file = files.get(id);
        for (Iterator<ClientInfo> it = file.owners(); it.hasNext(); ) {
            ClientInfo client = it.next();
            if (!client.isOnline()) {
                it.remove();
            }
        }
        return file;
    }

    @Override
    public int registerFile(String name, long size, ClientInfo owner) {
        int id = files.size();
        files.add(new FileInfo(files.size(), name, size, owner));
        return id;
    }

    @Override
    public void addOwner(int fileId, ClientInfo client) {
        files.get(fileId).addOwner(client);
    }

    @Override
    public int filesNumber() {
        return files.size();
    }

    @Override
    public Iterator<FileInfo> iterator() {
        return files.iterator();
    }
}
