package ru.spb.hse.httpproxy.parser;

import com.sun.org.apache.bcel.internal.generic.NEW;

import java.util.*;

public class HTTPMessage {
    public static final String NEW_LINE = "\r\n";
    public static final Set<String> SUPPORTED_METHODS = new HashSet<>(Arrays.asList("GET", "POST", "HEAD"));


    private String requestLine;
    private HashMap<String, String> headers;
    private ArrayList<Byte> body;

    public HTTPMessage() {
        requestLine = "";
        headers = new HashMap<>();
        body = new ArrayList<>();
    }

    public void setRequestLine(String requestLine) {
        this.requestLine = requestLine;
    }

    public void addHeaderField(String headerName, String headerValue) {
        headers.put(headerName, headerValue);
    }

    public boolean addToBody(byte[] toAdd) {
        for (byte b : toAdd) {
            body.add(b);
        }
        if (isChunked()) {
            byte[] finish = ("0" + NEW_LINE + NEW_LINE).getBytes();
            if (body.size() >= finish.length) {
                List<Byte> bodyEnd = body.subList(body.size() - finish.length, body.size());
                boolean equal = true;
                for (int i = 0; i < finish.length; i++) {
                    if (finish[i] != bodyEnd.get(i)) {
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    body.remove(body.size() - 1);
                    body.remove(body.size() - 1);
                    return true;
                } else {
                    return false;
                }
            }
        }
        return body.size() == getBodyLength();
    }

    public Address getAddress() {
        if (!headers.containsKey("Host")) {
            return new Address(null, 0);
        }
        String[] splitted = headers.get("Host").split(":");
        return new Address(splitted[0], splitted.length > 1 ? Integer.parseInt(splitted[1]) : 80);
    }

    public boolean isSupported() {
        String method = requestLine.split(" ")[0];
        return SUPPORTED_METHODS.contains(method);
    }

    private boolean isChunked() {
        return headers.getOrDefault("Transfer-Encoding", "").equals("chunked");
    }

    private int getBodyLength() {
        return Integer.parseInt(headers.getOrDefault("Content-Length", "0"));
    }

}
