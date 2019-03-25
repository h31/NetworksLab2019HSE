package ru.hse.alyokhina.server;

import javax.annotation.Nonnull;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.NoSuchElementException;

public class ReadWriteHelper {
    public enum ContentType {JSON, TEXT}

    public static void writeResponse(@Nonnull final String statusCode,
                        @Nonnull final String body,
                        @Nonnull final ContentType contentType,
                        @Nonnull final BufferedWriter writer) throws IOException, NoSuchElementException {
        writer.write("HTTP/1.1 " + statusCode + "\n");
        writer.write("Content-Type: " + convertContentType(contentType) + "\n" +
                "Content-Length: " + body.length() + "\n");
        writer.write("\n");
        writer.write(body);
        writer.flush();
    }

    @Nonnull
    public static String convertContentType(@Nonnull final ContentType contentType) throws NoSuchElementException {
        switch (contentType) {
            case JSON:
                return "application/json";
            case TEXT:
                return "text/plain";
            default:
                throw new NoSuchElementException("Failed to convert " + contentType);
        }
    }

    public static String readPath(@Nonnull final BufferedReader in) throws IOException {
        return in.readLine();
    }

    @Nonnull
    public static Map<String, String> readHeaders(@Nonnull final BufferedReader in) throws IOException, IllegalArgumentException {
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
    public static String readBody(@Nonnull final BufferedReader in, @Nonnull final Map<String, String> headers) throws IOException,
            IllegalArgumentException {
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
}
