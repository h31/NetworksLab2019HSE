package itmo2018.se;

import sun.net.spi.nameservice.dns.DNSNameService;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
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
import java.util.concurrent.*;

//TODO добавить обработку исключений
public class Server {
    public static void main(String[] args) throws IOException {
        new Server().run();
    }

    private Selector selector;
    private ServerSocketChannel serverSocketChannel;
    private ScheduledExecutorService closer = Executors.newScheduledThreadPool(1);

    public void run() throws IOException {
        serverSocketChannel = ServerSocketChannel.open();
        InetSocketAddress socketAddress = new InetSocketAddress("127.0.0.1", 8081);
        serverSocketChannel.bind(socketAddress);
        selector = Selector.open();

        serverSocketChannel.configureBlocking(false);
        serverSocketChannel.register(selector, SelectionKey.OP_ACCEPT);

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
                        holder.getInfo().disconect();
                        System.out.println(channel.getRemoteAddress() + " is closed");
                        channel.close();
                    }
                    buffer.flip();
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
//        closer.shutdownNow();
    }

    private void registerClient() throws IOException {
        System.out.println("Clients number:" + selector.keys().size());
        SocketChannel channel = serverSocketChannel.accept();

        byte[] ip = channel.socket().getInetAddress().getAddress();
        short port = (short) channel.socket().getPort();
        Future<Void> closeTask = closer.schedule(new Closer(channel), 5, TimeUnit.SECONDS);
        ClientInfo info = new ClientInfo(ip, port, closeTask);

        channel.configureBlocking(false);
        channel.register(selector, SelectionKey.OP_READ, new ClientDataHolder(info));
        System.out.println("new client");
    }
}
