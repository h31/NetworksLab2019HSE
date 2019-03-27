package ru.spb.hse.httpproxy.parser;

import com.sun.org.apache.bcel.internal.generic.NEW;

import java.util.*;
import java.util.stream.Collectors;

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

    public byte[] getBytes() {
        String message = requestLine + NEW_LINE;
        message += headers.entrySet().stream()
                .map(entry -> entry.getKey() + ": " + entry.getValue())
                .collect(Collectors.joining(NEW_LINE));
        message += NEW_LINE + NEW_LINE;

        ArrayList<Byte> result = new ArrayList<>();

        for (byte b : message.getBytes()) {
            result.add(b);
        }

        if (getBodyLength() > 0 || isChunked()) {
            result.addAll(body);
            for (byte b : NEW_LINE.getBytes()) {
                result.add(b);
            }
        }
        byte[] answer = new byte[result.size()];

        for(int i = 0; i < answer.length; i++) {
            answer[i] = result.get(i);
        }
        return answer;
    }

    public boolean canBeCached() {
        if (!headers.containsKey("Cache-Control")) {
            return true;
        }
        String value = headers.get("Cache-Control");
        return !value.contains("no-cache") && !value.contains("no-store");
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
