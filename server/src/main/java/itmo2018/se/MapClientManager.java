package itmo2018.se;


import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

public class MapClientManager implements ClientManager {
    private Map<String, ClientProperty> clients = new HashMap<>();

    @Override
    public ClientProperty getClient(String adress) {
        return clients.get(adress);
    }

    @Override
    public void registerClient(String adress, SocketChannel channel) {
        clients.put(adress, new ClientProperty(channel));
    }

    @Override
    public void addFiles(String adress, Map<String, List<Integer>> files) {
        ClientProperty clientProperty = clients.get(adress);
        files.forEach(clientProperty::registerFiles);
    }

    @Override
    public void unregisterClient(String adress) {
        clients.remove(adress);
    }

    @Override
    public Iterator<ClientProperty> iterator() {
        return clients.values().iterator();
    }
}
