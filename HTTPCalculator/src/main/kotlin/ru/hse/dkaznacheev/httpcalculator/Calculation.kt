package ru.hse.dkaznacheev.httpcalculator

import java.net.URLEncoder
import kotlin.math.roundToLong

data class Calculation(val lhs: Long, val rhs: Long?, val operation: String) {

    fun toURL(): String {
        val builder = StringBuilder()
        builder.append("/calc?")
        builder.append("lhs=$lhs")
        if (rhs != null) {
            builder.append("&rhs=$rhs")
        }
        builder.append("&op=${URLEncoder.encode(operation, "utf-8")}")
        return builder.toString()
    }

    fun evaluate(): Long? {
        if (operation in quickOperations) {
            if (rhs == null)
                return null
            return when(operation) {
                "+" -> lhs + rhs
                "-" -> lhs - rhs
                "/" -> if (rhs == 0.toLong()) null else lhs / rhs
                "*" -> lhs * rhs
                else -> null
            }
        }

        if (operation == "fact") {
            return (1..lhs).reduce { acc, arg -> acc * arg }
        }

        if (operation == "sqrt") {
            return Math.sqrt(lhs.toDouble()).roundToLong()
        }

        return null
    }

    fun isLong(): Boolean {
        return operation in longOperations
    }

    companion object {
        private val longOperations = listOf("sqrt", "fact")
        private val quickOperations = listOf("+", "-", '*', "/")
        private val availableOperations = longOperations.plus(quickOperations)

        fun parseCalculationfromURL(url: String): Calculation? {
            val urlParts = url.split("?")

            if (urlParts.size < 2 || urlParts[0] != "/calc")
                return null

            val params = urlParts[1].split("&")

            var lhs: Long? = null
            var rhs: Long? = null
            var operation: String? = null

            for (param in params) {
                val paramParts = param.split("=")
                if (paramParts.size < 2)
                    return null
                when (paramParts[0]) {
                    "lhs" -> lhs = paramParts[1].toLongOrNull() ?: return null
                    "rhs" -> rhs = paramParts[1].toLongOrNull() ?: return null
                    "op" -> operation = paramParts[1]
                }
            }

            if (lhs == null || operation == null)
                return null

            if (operation !in availableOperations)
                return null

            return Calculation(lhs, rhs, operation)
        }

        private fun parseOneArgCalculation(line: String, operation: String): Calculation? {
            val lhs = line.drop(operation.length + 1).toLongOrNull() ?: return null
            return Calculation(lhs, null, operation)
        }

        private fun parseTwoArgCalculation(line: String): Calculation? {
            val tokens = line.split(" ")
            if (tokens.size != 3)
                return null
            val lhs = tokens[0].toLongOrNull() ?: return null
            val operation = tokens[1]
            val rhs = tokens[2].toLongOrNull() ?: return null

            return Calculation(lhs, rhs, operation)
        }

        fun parseCalculationFromLine(line: String): Calculation? {
            if (line.startsWith("fact ")) {
                return parseOneArgCalculation(line, "fact")
            }
            if (line.startsWith("sqrt ")) {
                return parseOneArgCalculation(line, "sqrt")

            }

            return parseTwoArgCalculation(line)
        }
    }
}