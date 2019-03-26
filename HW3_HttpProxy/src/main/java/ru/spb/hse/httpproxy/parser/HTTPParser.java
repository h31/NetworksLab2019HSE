package ru.spb.hse.httpproxy.parser;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class HTTPParser {

    private HTTPMessage message;
    private List<Byte> head;

    public HTTPParser(InputStream input) throws IOException {
        message = new HTTPMessage();
        head = new ArrayList<>();

        //readHeader(input);

        byte[] buffer = new byte[1024];
        int read = 0;
        while ((read = input.read(buffer)) != -1) {
            if (append(Arrays.copyOfRange(buffer, 0, read))) {
                break;
            }
        }
    }

    public HTTPMessage getMessage() {
        return message;
    }

    private boolean append(byte[] toAdd) {
        if (head == null) {
            return parseBody(toAdd);
        }

        for (byte b : toAdd) {
            head.add(b);
        }

        int doubleLine = findDoubleNewLine();
        if (doubleLine == -1) {
            return false;
        }

        List<Byte> remain = head.subList(doubleLine + (HTTPMessage.NEW_LINE + HTTPMessage.NEW_LINE).getBytes().length, head.size());
        head = head.subList(0, doubleLine);
        parseHead();
        toAdd = new byte[remain.size()];
        for (int i = 0; i < toAdd.length; i++) {
            toAdd[i] = remain.get(i);
        }
        return parseBody(toAdd);
    }

    private void parseHead() {
        byte[] all = new byte[head.size()];
        for (int i = 0; i < all.length; i++) {
            all[i] = head.get(i);
        }
        String[] headers = new String(all).split(HTTPMessage.NEW_LINE);
        message.setRequestLine(headers[0]);
        for (int i = 1; i < headers.length; i++) {
            int endOfKey = headers[i].indexOf(": ");
            message.addHeaderField(headers[i].substring(0, endOfKey), headers[i].substring(endOfKey + 2));
        }
        head = null;
    }

    private boolean parseBody(byte[] toAdd) {
        return message.addToBody(toAdd);
    }

    private int findDoubleNewLine() {
        byte[] finish = (HTTPMessage.NEW_LINE + HTTPMessage.NEW_LINE).getBytes();
        for (int i = 0; i < head.size() - finish.length; i++) {
            boolean equal = true;
            for (int j = 0; j < finish.length; j++) {
                if (finish[i] != head.get(i + j)) {
                    equal = false;
                    break;
                }
            }
            if (equal) {
                return i;
            }
        }
        return -1;
    }
}
