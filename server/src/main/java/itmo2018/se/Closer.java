package itmo2018.se;

import java.nio.channels.SocketChannel;
import java.util.concurrent.Callable;

public class Closer implements Callable<Void> {
    private SocketChannel channel;

    Closer(SocketChannel channel) {
        this.channel = channel;
    }

    @Override
    public Void call() throws Exception {
        //TODO сделать демоном
        System.out.println(channel.getRemoteAddress() + " is interupt");
        channel.close();
        return null;
    }
}
