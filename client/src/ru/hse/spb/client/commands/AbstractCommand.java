package ru.hse.spb.client.commands;

import org.jetbrains.annotations.NotNull;

import java.util.List;

public interface AbstractCommand {
    void processCommand(@NotNull List<@NotNull String> args) throws CommandException;

    String getInfo();
}
