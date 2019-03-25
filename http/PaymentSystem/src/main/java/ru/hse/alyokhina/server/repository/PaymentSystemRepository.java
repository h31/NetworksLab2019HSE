package ru.hse.alyokhina.server.repository;


import ru.hse.alyokhina.server.dto.*;
import ru.hse.alyokhina.server.exception.NotAuthorizedException;

import javax.annotation.Nonnull;

import java.util.*;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class PaymentSystemRepository {
    private long defaultMoneyCount = 100;

    private static ReadWriteLock readWriteLockData = new ReentrantReadWriteLock();
    private static Lock readLockData = readWriteLockData.readLock();
    private static Lock writeLockData = readWriteLockData.writeLock();

    private static ReadWriteLock readWriteLockRequest = new ReentrantReadWriteLock();
    private static Lock readLockRequest = readWriteLockRequest.readLock();
    private static Lock writeLockRequest = readWriteLockRequest.writeLock();

    private static Map<String, User> data = new HashMap<>();
    private static Map<String, Map<String, Request>> requests = new HashMap<>();

    @Nonnull

    public DefaultResponse createUser(@Nonnull final UserInfo request) {
        writeLockData.lock();
        if (data.get(request.getLogin()) != null) {
            writeLockData.unlock();
            throw new IllegalArgumentException("This login " + request.getLogin() + " is already in use.");
        }
        final User newUser = new User(request.getLogin(), request.getPassword(), defaultMoneyCount);
        data.put(request.getLogin(), newUser);
        writeLockData.unlock();
        writeLockRequest.lock();
        requests.put(request.getLogin(), new HashMap<>());
        writeLockRequest.unlock();
        return new DefaultResponse(Status.OK, "", null);
    }

    @Nonnull
    public Collection<String> getUsers() {
        readLockData.lock();
        final Set<String> users = data.keySet();
        readLockData.unlock();
        return users;
    }

    @Nonnull
    public DefaultResponse sendTransfer(@Nonnull final TransferRequest transferRequest) throws NotAuthorizedException {
        valid(transferRequest.getFromLogin(), transferRequest.getFromPassword(), transferRequest.getToLogin());
        boolean result = transfer(transferRequest.getFromLogin(), transferRequest.getFromLogin(), transferRequest.getCount());
        return result
                ? new DefaultResponse(Status.OK, "", null)
                : new DefaultResponse(Status.ERROR, "not enough money to transfer", null);
    }


    @Nonnull
    public DefaultResponse requestTransfer(@Nonnull final TransferRequest transferRequest) throws NotAuthorizedException {
        valid(transferRequest.getFromLogin(), transferRequest.getFromPassword(), transferRequest.getToLogin());
        writeLockRequest.lock();
        final Map<String, Request> requestsForTo = requests.get(transferRequest.getToLogin());
        final String key = generateKey(requestsForTo);
        requestsForTo.put(key, new Request(transferRequest.getFromLogin(), transferRequest.getCount()));
        writeLockRequest.unlock();
        return new DefaultResponse(Status.OK, "", key);
    }

    @Nonnull
    public DefaultResponse acceptRequest(@Nonnull TransferRequestAccept requestAccept) throws NotAuthorizedException {
        valid(requestAccept.getLogin(), requestAccept.getPassword(), null);

        writeLockRequest.lock();
        final Map<String, Request> requestsForUser = requests.get(requestAccept.getLogin());
        final Request request = requestsForUser.get(requestAccept.getKey());
        if (request == null) {
            writeLockRequest.unlock();
            return new DefaultResponse(Status.ERROR, "Request " + requestAccept.getKey() + " not found", null);
        }
        if (requestAccept.getActionType() == ActionType.ACCEPT) {
            boolean result = transfer(requestAccept.getLogin(), request.loginFrom, request.count);
            if (!result) {
                writeLockRequest.unlock();
                new DefaultResponse(Status.ERROR, "not enough money to transfer", null);
            }
        }
        requestsForUser.remove(requestAccept.getKey());
        writeLockRequest.unlock();
        return new DefaultResponse(Status.OK, "", null);
    }

    public WalletInfo getCountMoney(@Nonnull final UserInfo userInfo) throws NotAuthorizedException {
        valid(userInfo.getLogin(), userInfo.getPassword(), null);
        readLockData.lock();
        long countMoney = data.get(userInfo.getLogin()).count;
        readLockData.unlock();
        return new WalletInfo(userInfo.getLogin(), countMoney);
    }

    private String generateKey(@Nonnull final Map<String, ?> map) {
        Random random = new Random();
        String newKey;
        while (true) {
            char[] chars = "abcdefghijklmnopqrstuvwxyz1234567890".toCharArray();
            StringBuilder sb = new StringBuilder(20);
            for (int i = 0; i < 20; i++) {
                char c = chars[random.nextInt(chars.length)];
                sb.append(c);
            }
            newKey = sb.toString();
            if (map.get(newKey) == null) {
                return newKey;
            }
        }
    }

    private boolean transfer(@Nonnull final String loginFrom,
                             @Nonnull final String loginTo,
                             final long count) {
        writeLockData.lock();
        final User userFrom = data.get(loginFrom);
        final User userTo = data.get(loginTo);
        if (userFrom.count < count) {
            writeLockData.unlock();
            return false;
        }
        data.put(userFrom.login, new User(userFrom.login,
                userFrom.password,
                userFrom.count - count));
        data.put(userTo.login, new User(userTo.login, userTo.password, userTo.count + count));
        writeLockData.unlock();
        return true;
    }

    private void valid(@Nonnull final String loginFrom,
                       @Nonnull final String password,
                       final String loginTo) throws NotAuthorizedException {
        readLockData.lock();
        final User userFrom = data.get(loginFrom);
        readLockData.unlock();
        if (userFrom == null) {

            throw new NotAuthorizedException("user " + loginFrom + " not registered");
        }

        if (loginTo != null && data.get(loginTo) == null) {
            throw new NotAuthorizedException("user " + loginTo + " not registered");
        }
        if (!userFrom.password.equals(password)) {

            throw new NotAuthorizedException("wrong password for " + loginFrom);
        }
    }

    private class User {
        private final String login;
        private final String password;
        private final long count;

        private User(@Nonnull final String login, @Nonnull final String password, long count) {
            this.login = login;
            this.password = password;
            this.count = count;
        }


        @Override
        public int hashCode() {
            return Objects.hash(login, password, count);
        }

        @Override
        public boolean equals(Object o) {
            if (!(o instanceof User)) {
                return false;
            }
            final User that = (User) o;
            return Objects.equals(this.login, that.login)
                    && Objects.equals(this.password, that.password)
                    && this.count == that.count;
        }
    }


    private class Request {
        private final String loginFrom;
        private final long count;

        private Request(@Nonnull final String loginFrom, long count) {
            this.loginFrom = loginFrom;
            this.count = count;
        }


        @Override
        public int hashCode() {
            return Objects.hash(loginFrom, count);
        }

        @Override
        public boolean equals(Object o) {
            if (!(o instanceof Request)) {
                return false;
            }
            final Request that = (Request) o;
            return Objects.equals(this.loginFrom, that.loginFrom)
                    && this.count == that.count;
        }
    }
}
