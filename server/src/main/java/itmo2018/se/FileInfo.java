package itmo2018.se;

import java.util.*;

public class FileInfo {
    private int id;
    private String name;
    private Set<ClientInfo> owners = new HashSet<>();

    public FileInfo(int id, String name) {
        this.id = id;
        this.name = name;
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

    public Iterator<ClientInfo> owners() {
        return owners.iterator();
    }
}
