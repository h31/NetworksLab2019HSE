package itmo2018.se;

import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class FileInfo {
    private int id;
    private String name;
    private Map<ClientInfo, List<Integer>> owners = new HashMap<>();

    public FileInfo(int id, String name) {
        this.id = id;
        this.name = name;
    }

    public void addOwner(ClientInfo client, List<Integer> parts) {
        owners.put(client, parts);
    }

    public void removeOwner(ClientInfo client) {
        owners.remove(client);
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Iterator<ClientInfo> owners() {
        return owners.keySet().iterator();
    }
}
