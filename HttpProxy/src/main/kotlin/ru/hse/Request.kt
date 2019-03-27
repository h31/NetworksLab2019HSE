package ru.hse

data class Request(
    val method: String, val host: String,
    val port: Int,
    val protocol: String,
    val url: String,
    val data: String,
    val auth: String?
)