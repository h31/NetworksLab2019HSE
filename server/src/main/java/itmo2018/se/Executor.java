package itmo2018.se;

import com.sun.xml.internal.messaging.saaj.util.ByteOutputStream;

import java.io.*;
import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.concurrent.Callable;

public class Executor implements Callable<Void> {
    private DataInputStream content;
    private ClientDataHolder holder;
    private FileManager fileManager;

    public Executor(byte[] bytes, ClientDataHolder holder, FileManager fileManager) {
        this.content = new DataInputStream(new ByteArrayInputStream(bytes));
        this.holder = holder;
        this.fileManager = fileManager;
    }

    @Override
    public Void call() throws Exception {
        byte cmd = content.readByte();
        switch (cmd) {
            case 1:
                executeList();
                break;
            case 2:
                executeUpload();
                break;
            case 3:
                executeSources();
                break;
            case 4:
                executeUpdate();
                break;
        }
        return null;
    }

    private void executeList() throws IOException {
        System.out.println("list");

        ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
        DataOutputStream out = new DataOutputStream(byteStream);
        out.writeInt(fileManager.filesNumber());
        for (FileInfo file : fileManager) {
            out.writeInt(file.getId());
            out.writeUTF(file.getName());
            out.writeLong(file.getSize());
        }

        ByteBuffer response = ByteBuffer.allocate(out.size());
        response.put(byteStream.toByteArray());
        response.flip();
        holder.addResponse(response);
    }

    private void executeUpload() throws IOException {
        System.out.println("upload");

        String name = content.readUTF();
        long size = content.readLong();

        System.out.println(name + " " + size);
        int id = fileManager.registerFile(name, size);

        ByteBuffer response = ByteBuffer.allocate(4);
        response.putInt(id);
        response.flip();
        holder.addResponse(response);
    }

    private void executeSources() throws IOException {
        System.out.println("sources");

        int id = content.readInt();

        FileInfo file = fileManager.getFile(id);
        ByteBuffer response = ByteBuffer.allocate(4 + file.ownersNumber() * (4 + 2));
        response.putInt(file.ownersNumber());
        for (Iterator<ClientInfo> it = file.owners(); it.hasNext(); ) {
            ClientInfo client = it.next();
            response.put(client.getIp());
            response.putShort(client.getPort());
        }
        response.flip();
        holder.addResponse(response);
    }

    private void executeUpdate() throws IOException {
        System.out.println("update");

        ClientInfo client = holder.getClientInfo();
        client.updateCloseTask();
        short port = content.readShort();
        client.updateSharingPort(port);
        int count = content.readInt();
        for (int i = 0; i < count; i++) {
            int fileId = content.readInt();
            fileManager.getFile(fileId).addOwner(client);
        }

        ByteBuffer response = ByteBuffer.allocate(1);
        response.put((byte) 1);
        response.flip();
        holder.addResponse(response);
    }
}
