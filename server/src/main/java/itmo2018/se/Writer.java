package itmo2018.se;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.ConcurrentLinkedQueue;

public class Writer implements Runnable {
    private Selector selector;
    private Queue<ClientDataHolder> clientQueue = new ConcurrentLinkedQueue<>();

    public Writer(Selector selector) {
        this.selector = selector;
    }

    public void registerClient(ClientDataHolder client) {
        clientQueue.add(client);
        selector.wakeup();
    }

    @Override
    public void run() {
        try {
            while (selector.select() > -1) {
                //исправить на случай многократного чтения
                if (clientQueue.size() > 0) {
                    ClientDataHolder data = clientQueue.poll();
                    data.getClientInfo().getChannel().register(selector, SelectionKey.OP_WRITE, data);
                    continue;
                }
                Set<SelectionKey> selectedKeys = selector.selectedKeys();
                Iterator<SelectionKey> keyIterator = selectedKeys.iterator();
                while (keyIterator.hasNext()) {
                    SelectionKey key = keyIterator.next();
                    if (key.isWritable()) {
                        SocketChannel channel = (SocketChannel) key.channel();
                        ClientDataHolder responceQueue = (ClientDataHolder) key.attachment();
                        ByteBuffer responce = responceQueue.getResponse();
                        if (responce != null) {
                            System.out.println("Send responce!!!!");
                            channel.write(responce);
                        } else {
                            key.cancel();
                        }
                    }
                    keyIterator.remove();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
