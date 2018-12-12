package itmo2018.se;

import java.util.*;

public class FileInfo {
    private int id;
    private String name;
    private long size;
    private Set<ClientInfo> owners = new HashSet<>();

    public FileInfo(int id, String name, long size, ClientInfo owner) {
        this.id = id;
        this.name = name;
        this.size = size;
        this.owners.add(owner);
    }

    public void addOwner(ClientInfo client) {
        owners.add(client);
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public long getSize() {
        return size;
    }

    public int ownersNumber() {
        return owners.size();
    }

    public Iterator<ClientInfo> owners() {
        return owners.iterator();
    }
}
