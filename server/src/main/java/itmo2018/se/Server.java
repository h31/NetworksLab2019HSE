package itmo2018.se;

import java.io.File;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Iterator;
import java.util.Queue;
import java.util.Set;
import java.util.concurrent.*;

//TODO добавить обработку исключений
public class Server {
    public static void main(String[] args) throws IOException {
        new Server().run();
    }

    public Server() throws IOException {
        File metaData = new File("metadata");
        if (!metaData.exists()) {
            metaData.createNewFile();
        }
        Files.lines(metaData.toPath()).map(it -> it.split("\t"))
                .forEach(it -> fileManager.setFile(Integer.parseInt(it[0]), it[1], Long.parseLong(it[2])));
    }

    private Selector selector;
    private ServerSocketChannel serverSocketChannel;
    private ScheduledExecutorService closer = Executors.newScheduledThreadPool(1);
    private FileManager fileManager = new ListFileManager();

    public void run() throws IOException {
        serverSocketChannel = ServerSocketChannel.open();
        Socket getIp = new Socket("2ip.ru", 80);
        InetSocketAddress socketAddress = new InetSocketAddress(getIp.getLocalAddress().getHostAddress(), 8081);
        serverSocketChannel.bind(socketAddress);
        System.out.println("server adress: " + serverSocketChannel.socket().getLocalSocketAddress());
        selector = Selector.open();
        Selector writerSelector = Selector.open();
        Writer writer = new Writer(writerSelector);
        Thread writerThread = new Thread(writer);
        writerThread.setDaemon(true);
        writerThread.start();

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
                    ClientDataHolder client = (ClientDataHolder) key.attachment();
                    ByteBuffer buffer = ByteBuffer.allocate(1024);
                    if (channel.read(buffer) == -1) {
                        System.out.println(channel.getRemoteAddress() + " is closed");
                        client.getClientInfo().disconect();
                        channel.close();
                    }
                    buffer.flip();
                    client.read(buffer);
                    while (client.requestIsReady()) {
                        pool.submit(new Executor(client.getRequest(), client, fileManager, writer));
                        client.resetRequest();
                        client.read(buffer);
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

        System.out.println(channel.socket().getRemoteSocketAddress());

        channel.configureBlocking(false);
        channel.register(selector, SelectionKey.OP_READ, new ClientDataHolder(info));
        System.out.println("new client");
    }
}
