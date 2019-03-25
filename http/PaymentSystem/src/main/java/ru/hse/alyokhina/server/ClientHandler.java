package ru.hse.alyokhina.server;


import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import ru.hse.alyokhina.server.dto.*;
import ru.hse.alyokhina.server.repository.PaymentSystemRepository;
import ru.hse.alyokhina.server.exception.NotAuthorizedException;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.Map;
import javax.annotation.Nonnull;


import static ru.hse.alyokhina.server.ReadWriteHelper.readBody;
import static ru.hse.alyokhina.server.ReadWriteHelper.writeResponse;
import static ru.hse.alyokhina.server.ReadWriteHelper.ContentType;
import static ru.hse.alyokhina.server.ReadWriteHelper.readHeaders;
import static ru.hse.alyokhina.server.ReadWriteHelper.readPath;


public class ClientHandler implements Runnable {
    private final Socket client;
    private final ObjectMapper _J = new ObjectMapper();
    private final PaymentSystemRepository paymentSystem = new PaymentSystemRepository();

    public ClientHandler(@Nonnull Socket client) {
        this.client = client;
    }

    public void run() {
        try {
            final BufferedReader inReader = new BufferedReader(
                    new InputStreamReader(client.getInputStream(), "UTF-8"));
            final BufferedWriter outWriter = new BufferedWriter(
                    new OutputStreamWriter(client.getOutputStream(), "UTF-8"));
            String response;
            String status;
            ContentType contentType;
            try {
                final String path = readPath(inReader);
                final Map<String, String> headers = readHeaders(inReader);
                final String body = readBody(inReader, headers);
                System.out.println(path);
                System.out.println(headers);
                System.out.println(body);
                response = apply(path, body);
                status = "200";
                contentType = ContentType.JSON;
            } catch (IllegalArgumentException e) {
                status = "400";
                response = e.getMessage();
                contentType = ContentType.TEXT;
            } catch (NotAuthorizedException e) {
                status = "401";
                response = e.getMessage();
                contentType = ContentType.TEXT;
            } catch (ClassNotFoundException e) {
                status = "404";
                response = e.getMessage();
                contentType = ContentType.TEXT;
            } catch (Throwable e) {
                status = "500";
                response = e.getMessage();
                contentType = ContentType.TEXT;
            }
            writeResponse(status, response, contentType, outWriter);
            client.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


    private String apply(@Nonnull final String path, @Nonnull final String body) throws JsonProcessingException
            , ClassNotFoundException
            , NotAuthorizedException {
        final String pathParts[] = path.split(" ");
        if (pathParts.length < 2) {
            throw new IllegalArgumentException("Failed to parse http path");
        }
        if (pathParts[0].toLowerCase().equals("post")
                && pathParts[1].equals("/user")) {
            UserInfo userInfo;
            try {
                userInfo = _J.readValue(body, UserInfo.class);
            } catch (IOException e) {
                throw new IllegalArgumentException("Failed to parse body as UserInfo", e);
            }
            final DefaultResponse response = paymentSystem.createUser(userInfo);
            return _J.writeValueAsString(response);
        } else if (pathParts[0].toLowerCase().equals("get")
                && pathParts[1].equals("/users")) {
            return _J.writeValueAsString(paymentSystem.getUsers());
        } else if (pathParts[0].toLowerCase().equals("post")
                && pathParts[1].equals("/send")) {
            TransferRequest transferRequest;
            try {
                transferRequest = _J.readValue(body, TransferRequest.class);
            } catch (IOException e) {
                throw new IllegalArgumentException("Failed to parse body as TransferRequest", e);
            }
            final DefaultResponse response = paymentSystem.sendTransfer(transferRequest);
            return _J.writeValueAsString(response);
        } else if (pathParts[0].toLowerCase().equals("post")
                && pathParts[1].equals("/request")) {
            TransferRequest transferRequest;
            try {
                transferRequest = _J.readValue(body, TransferRequest.class);
            } catch (IOException e) {
                throw new IllegalArgumentException("Failed to parse body as TransferRequest", e);
            }
            final DefaultResponse response = paymentSystem.requestTransfer(transferRequest);
            return _J.writeValueAsString(response);
        } else if (pathParts[0].toLowerCase().equals("post")
                && pathParts[1].equals("/accept")) {
            TransferRequestAccept transferRequestAccept;
            try {
                transferRequestAccept = _J.readValue(body, TransferRequestAccept.class);
            } catch (IOException e) {
                throw new IllegalArgumentException("Failed to parse body as TransferRequestAccept", e);
            }
            final DefaultResponse response = paymentSystem.acceptRequest(transferRequestAccept);
            return _J.writeValueAsString(response);
        } else if (pathParts[0].toLowerCase().equals("post")
                && pathParts[1].equals("/wallet")) {
            UserInfo userInfo;
            try {
                userInfo = _J.readValue(body, UserInfo.class);
            } catch (IOException e) {
                throw new IllegalArgumentException("Failed to parse body as UserInfo", e);
            }
            final WalletInfo response = paymentSystem.getCountMoney(userInfo);
            return _J.writeValueAsString(response);
        }
        throw new ClassNotFoundException(pathParts[0] + " " + pathParts[1] + " not found");
    }
}
