package itmo2018.se;

import java.nio.channels.SocketChannel;
import java.util.List;
import java.util.Map;

public interface ClientManager extends Iterable<ClientProperty> {
    ClientProperty getClient(String adress);

    void registerClient(String adress, SocketChannel channel);

    void addFiles(String adress, Map<String, List<Integer>> files);

    void unregisterClient(String adress);
}
