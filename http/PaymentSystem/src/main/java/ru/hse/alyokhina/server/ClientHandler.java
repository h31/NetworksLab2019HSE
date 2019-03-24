package ru.hse.alyokhina.server;


import com.sun.istack.internal.NotNull;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.HashMap;
import java.util.Map;
import javax.annotation.Nonnull;


public class ClientHandler implements Runnable {
    private final Socket client;


    public ClientHandler(@Nonnull Socket client) {
        this.client = client;
    }

    public void run() {
        try {
            final BufferedReader inReader = new BufferedReader(
                    new InputStreamReader(client.getInputStream(), "UTF-8"));
            final BufferedWriter outWriter = new BufferedWriter(
                    new OutputStreamWriter(client.getOutputStream(), "UTF-8"));

            final String path = readPath(inReader);
            final Map<String, String> headers = readHeaders(inReader);
            final String body = readBody(inReader, headers);
            System.out.println(path);
            System.out.println(headers);
            System.out.println(body);
            final String response = " Hello world!";
            respond("200", response, outWriter);
            client.close();
        } catch (IOException e) {
            System.out.println("knlkjnlk");
        }

    }


    private void respond(String statusCode, String body, BufferedWriter writer) throws IOException {


        writer.write("HTTP/1.1 $statusCode\n");
        writer.write(
                "Content-Type: text/plain; charset=utf-8\n" +
                        "Server: calculator/1.0\n" +
                        "Content-Length: ${body.length}\n");
        writer.write("\n");
        writer.write(body);
        writer.flush();
    }

    private String readPath(@Nonnull final BufferedReader in) throws IOException {
        return in.readLine();
    }

    @Nonnull
    private Map<String, String> readHeaders(@Nonnull final BufferedReader in) throws IOException {
        final Map<String, String> headers = new HashMap<String, String>();
        while (true) {
            final String curLine = in.readLine();
            if ("".equals(curLine)) {
                break;
            }
            final String[] keyAndValues = curLine.split(": ");
            if (keyAndValues.length != 2) {
                throw new IllegalArgumentException("Failed to parse http headers");
            }
            headers.put(keyAndValues[0].toLowerCase(), keyAndValues[1]);
        }
        return headers;
    }

    @Nonnull
    private String readBody(@Nonnull final BufferedReader in, @Nonnull final Map<String, String> headers) throws IOException {
        if (headers.get("content-length") == null) {
            return "";
        }
        try {
            final int contentLength = Integer.parseInt(headers.get("content-length"));
            final char[] buffer = new char[contentLength];
            final int countRead = in.read(buffer);
            if (countRead != contentLength) {
                throw new IllegalArgumentException("Error during reading body, content-length = " + contentLength);
            }
            return new String(buffer);
        } catch (NumberFormatException e) {
            throw new IllegalArgumentException("Failed to parse content-length ", e);
        }
    }

    private void writeString(@Nonnull final String value, final OutputStreamWriter outWriter) throws IOException {
        outWriter.write(value);
    }
}
