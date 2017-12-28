package org.itmo.torrent.network.messages.tracker;

import org.itmo.torrent.network.messages.Message;
import org.jetbrains.annotations.NotNull;

import java.util.Set;

public interface Update extends Message {
    byte ID = 4;
    int UPDATE_PERIOD = 10 * 60;  // millis

    class Request implements Update {
        private final short clientPort;

        @NotNull
        private final Set<Integer> filesIds;

        public Request(final short clientPort, @NotNull final Set<Integer> filesIds) {
            this.clientPort = clientPort;
            this.filesIds = filesIds;
        }

        public short getClientPort() {
            return clientPort;
        }

        @NotNull
        public Set<Integer> getFilesIds() {
            return filesIds;
        }

        @NotNull
        @Override
        public String toString() {
            return String.format("UpdateRequest{clientPort=%d, count=%d, filesIds=%s}",
                    clientPort, filesIds.size(), filesIds);
        }
    }

    class Response implements Update {
        private final boolean status;

        public Response(final boolean status) {
            this.status = status;
        }

        public boolean getStatus() {
            return status;
        }

        @NotNull
        @Override
        public String toString() {
            return String.format("UpdateResponse{status=%b}", status);
        }
    }
}
