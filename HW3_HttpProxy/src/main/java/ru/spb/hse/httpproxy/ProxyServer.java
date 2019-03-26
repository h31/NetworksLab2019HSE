package ru.spb.hse.httpproxy;

import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import ru.spb.hse.httpproxy.parser.Address;
import ru.spb.hse.httpproxy.parser.HTTPMessage;
import ru.spb.hse.httpproxy.parser.HTTPParser;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.time.Duration;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Logger;

public class ProxyServer implements AutoCloseable{
    private static final Logger LOGGER = Logger.getLogger("Server");
    private ServerSocket socket;
    private Cache cache;
    private List<Thread> threads = new ArrayList<>();

    public ProxyServer(int port, int expirationTimeInSec, int cacheTime)
            throws IOException {
        socket = new ServerSocket(port);
        cache = new Cache(Duration.ofSeconds(expirationTimeInSec), cacheTime);
        LOGGER.info("Server socket created");
    }

    public void start() throws IOException {
        LOGGER.info("Start");
        while (true) {
            Socket clientSocket = socket.accept();
            LOGGER.info("Accepted new client");
            processClient(clientSocket);
        }
    }

    private void processClient(Socket client) {
        Thread thread = new Thread(() -> {
            try {
                OutputStream out = client.getOutputStream();
                InputStream in = client.getInputStream();
                HTTPParser parser = new HTTPParser(in);
                HTTPMessage message = parser.getMessage();
                byte[] request = message.getBytes();
                LOGGER.info("Got request: " + new String(request));
                if (cache.contains(request)) {
                    byte[] response = cache.getResponse(request);
                    out.write(response);
                    out.flush();
                    return;
                }
                HTTPMessage responseFromHost = sendMessage(message);
                byte[] response = responseFromHost.getBytes();
                if (message.canBeCached()) {
                    cache.add(request, response);
                }
                out.write(response);
                LOGGER.info("Send response: " + new String(response));
            } catch (IOException e) {
                LOGGER.warning("Exception while processing client: " + e
                        .getMessage());
                try {
                    client.close();
                } catch (IOException e1) {
                    LOGGER.warning("Cannot close client: " + e1.getMessage());
                }
            }
        });
        thread.setDaemon(true);
        threads.add(thread);
        thread.start();
    }

    @Nullable
    private static HTTPMessage sendMessage(@NotNull HTTPMessage request)
            throws IOException {
        Address address = request.getAddress();
        LOGGER.info("Host: " + address.getHost() + " port: " + address
                .getPort());
        try(Socket socket = new Socket(address.getHost(), address.getPort())) {
            OutputStream outputStream = socket.getOutputStream();
            outputStream.write(request.getBytes());
            HTTPParser parser = new HTTPParser(socket.getInputStream());
            return parser.getMessage();
        }
    }

    @Override
    public void close() throws Exception {
        socket.close();
    }
}
