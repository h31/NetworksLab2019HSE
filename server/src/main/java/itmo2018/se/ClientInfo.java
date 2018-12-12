package itmo2018.se;

import com.sun.istack.internal.Nullable;

import java.nio.channels.SocketChannel;
import java.util.Arrays;
import java.util.Objects;
import java.util.concurrent.Callable;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class ClientInfo {
    private Future<Void> closeTask;
    private byte[] ip;
    private short port;
    private short sharingPort;
    private ScheduledExecutorService closeScheduled;
    private SocketChannel channel;

    public ClientInfo(byte[] ip, short port, SocketChannel channel, ScheduledExecutorService closeScheduled) {
        this.ip = ip;
        this.port = port;
        this.channel = channel;
        this.closeScheduled = closeScheduled;
        closeTask = startTimer();
    }

    public byte[] getIp() {
        return ip;
    }

    public short getPort() {
        return port;
    }

    public void updateCloseTask() {
        closeTask.cancel(false);
        closeTask = startTimer();
    }

    public boolean isOnline() {
        return !closeTask.isDone();
    }

//    public void disconect() {
//        closeTask.cancel(false);
//    }

    @Nullable
    public short getSharingPort() {
        return sharingPort;
    }

    public void updateSharingPort(short port) {
        this.sharingPort = port;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        ClientInfo that = (ClientInfo) o;
        return Arrays.equals(ip, that.ip) &&
                Objects.equals(port, that.port);
    }

    public SocketChannel getChannel() {
        return channel;
    }

    @Override
    public int hashCode() {
        return Objects.hash(ip, port);
    }

    private Future<Void> startTimer() {
        return closeScheduled.schedule(new Closer(), 5, TimeUnit.MINUTES);
    }

    private class Closer implements Callable<Void> {
        @Override
        public Void call() throws Exception {
            System.out.println(channel.getRemoteAddress() + " is interupt");
            channel.close();
            return null;
        }
    }
}
