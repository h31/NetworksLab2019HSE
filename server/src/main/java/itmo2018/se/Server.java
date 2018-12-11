package itmo2018.se;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.util.Iterator;
import java.util.Set;
import java.util.concurrent.*;

//TODO добавить обработку исключений
public class Server {
    public static void main(String[] args) throws IOException {
        new Server().run();
    }

    private Selector selector;
    private ServerSocketChannel serverSocketChannel;
    private ScheduledExecutorService closer = Executors.newScheduledThreadPool(1);
    private FileManager fileManager = new ListFileManager();

    public void run() throws IOException {
        serverSocketChannel = ServerSocketChannel.open();
        InetSocketAddress socketAddress = new InetSocketAddress("127.0.0.1", 8081);
        serverSocketChannel.bind(socketAddress);
        selector = Selector.open();

        serverSocketChannel.configureBlocking(false);
        serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);
        ExecutorService pool = Executors.newFixedThreadPool(8);

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
                    ClientDataHolder holder = (ClientDataHolder) key.attachment();
                    ByteBuffer buffer = ByteBuffer.allocate(1024);
                    if (channel.read(buffer) == -1) {
                        holder.getClientInfo().disconect();
                        System.out.println(channel.getRemoteAddress() + " is closed");
                        channel.close();
                    }
                    buffer.flip();
                    holder.read(buffer);
                    while (holder.requestIsReady()) {
                        pool.submit(new Executor(holder.getRequest(), holder, fileManager));
                        holder.resetRequest();
                        holder.read(buffer);
                        System.out.println("package is ready");
                    }
                }
                keyIterator.remove();
            }
        }
//        closer.shutdownNow();
    }

    private void registerClient() throws IOException {
        System.out.println("Clients number:" + selector.keys().size());
        SocketChannel channel = serverSocketChannel.accept();

        byte[] ip = channel.socket().getInetAddress().getAddress();
        short port = (short) channel.socket().getPort();
        ClientInfo info = new ClientInfo(ip, port, channel, closer);

        System.out.println(channel.socket().getLocalSocketAddress());

        channel.configureBlocking(false);
        channel.register(selector, SelectionKey.OP_READ, new ClientDataHolder(info));
        System.out.println("new client");
    }
}
