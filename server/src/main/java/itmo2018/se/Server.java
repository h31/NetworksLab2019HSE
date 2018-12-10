package itmo2018.se;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

//TODO добавить обработку исключений
public class Server {
    public static void main(String[] args) throws IOException {
        new Server().run();
    }

    private Selector selector;
    private ServerSocketChannel serverSocketChannel;
    private ClientManager clientManager = new MapClientManager();

    public void run() throws IOException {
        serverSocketChannel = ServerSocketChannel.open();
        InetSocketAddress socketAddress = new InetSocketAddress("localhost", 8081);
        serverSocketChannel.bind(socketAddress);
        selector = Selector.open();

        serverSocketChannel.configureBlocking(false);
        serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
        ExecutorService pool = Executors.newFixedThreadPool(8);
        Closer closer = new Closer(clientManager);
        pool.submit(closer);

        while (selector.select() > -1) {
            Set<SelectionKey> selectedKeys = selector.selectedKeys();
            Iterator<SelectionKey> keyIterator = selectedKeys.iterator();
            while (keyIterator.hasNext()) {
                SelectionKey key = keyIterator.next();
                if (key.isAcceptable()) {
                    registerClient();
                } else if (key.isReadable()) {
                    //TODO catch ClosedChannelException
                    SocketChannel channel = (SocketChannel) key.channel();
                    ByteBuffer buffer = ByteBuffer.allocate(1024);
                    if (channel.read(buffer) == -1) {
                        String adress = channel.getRemoteAddress().toString();
                        clientManager.unregisterClient(adress);
                        channel.close();
                        System.out.println(adress + " is closed");
                    }
                    buffer.flip();
                    ClientDataHolder holder = (ClientDataHolder) key.attachment();
                    holder.read(buffer);
                    while (holder.isReady()) {
                        //holder.getContent() и отправляю в тредпул
                        holder.reset();
                        holder.read(buffer);
                        System.out.println("package is ready");
                    }
                }
                keyIterator.remove();
            }
        }
        pool.shutdownNow();
    }

    private void registerClient() throws IOException {
        System.out.println("Clients number:" + selector.keys().size());
        SocketChannel channel = serverSocketChannel.accept();

        String adress = channel.getRemoteAddress().toString();
        clientManager.registerClient(adress, channel);

        channel.configureBlocking(false);
        channel.register(selector, SelectionKey.OP_READ, new ClientDataHolder());
        System.out.println("new client");
    }
}
