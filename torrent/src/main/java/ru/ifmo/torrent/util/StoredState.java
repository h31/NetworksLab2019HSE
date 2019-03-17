package ru.ifmo.torrent.util;

public interface StoredState {

    void restoreFromFile() throws TorrentException;

    void storeToFile() throws TorrentException;
}
