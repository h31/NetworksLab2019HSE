package itmo2018.se;

import java.util.*;

public class FileInfo {
    private int id;
    private String name;
    private long size;
    private Set<ClientInfo> owners = new HashSet<>();

    public FileInfo(int id, String name, long size, ClientInfo owner) {
        this(id, name, size);
        this.owners.add(owner);
    }

    public FileInfo(int id, String name, long size) {
        this.id = id;
        this.name = name;
        this.size = size;
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

    public boolean hasOwner(ClientInfo clientInfo) {
        return owners.contains(clientInfo);
    }

    public Iterator<ClientInfo> owners() {
        return owners.iterator();
    }
}
