package itmo2018.se;

import java.nio.channels.SocketChannel;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ClientProperty {
    private long lastUpdate;
    private Map<String, List<Integer>> files;
    private SocketChannel channel;

    public ClientProperty(SocketChannel channel) {
        this.channel = channel;
        this.lastUpdate = System.currentTimeMillis();
    }

    public void registerFiles(String name, List<Integer> parts) {
        files.put(name, parts);
    }

    public long lastUpdate() {
        return lastUpdate;
    }

    public SocketChannel getChannel() {
        return channel;
    }
}
