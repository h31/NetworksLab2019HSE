package itmo2018.se;

import java.nio.channels.SocketChannel;
import java.util.concurrent.Callable;

public class Closer implements Callable<Void> {
    private ClientManager clientManager;

    Closer(ClientManager clientManager) {
        this.clientManager = clientManager;
    }

    @Override
    public Void call() throws Exception {
        //TODO сделать демоном
        while (true) {
            long currentTime = System.currentTimeMillis();
            for (ClientProperty clientProperty : clientManager) {
                long trashTime = 5000;
                if (currentTime - clientProperty.lastUpdate() >= trashTime) {
                    SocketChannel channel = clientProperty.getChannel();
                    String adress = channel.getRemoteAddress().toString();
                    clientManager.unregisterClient(adress);
                    channel.close();
                    System.out.println(adress + " is interupted");
                }
            }
        }
    }
}
