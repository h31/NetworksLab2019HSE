#include <cstdint>
#include <cmath>
#include <unistd.h>
#include <cstdio>
#include <string>

#pragma once
namespace operations {
    using std::string;
    enum Type {
        Fact = 1,
        Sq = 2,
        Add = 3,
        Subs = 4,
        Mult = 5,
        Div = 6
    };


    Type typeFromString(const string &text) {
        if (text == "fact") {
            return Fact;
        }

        if (text == "sq") {
            return Sq;
        }

        if (text == "add") {
            return Add;
        }

        if (text == "sub") {
            return Subs;
        }

        if (text == "mul") {
            return Mult;
        }

        if (text == "div") {
            return Div;
        }

        perror("Invalid operation type");
    };

    string stringFromType(Type type) {
        switch (type) {
            case Type::Fact :
                return "factorial";
            case Type::Sq:
                return "square";
            case Type::Add:
                return "addition";
            case Subs:
                return "substract";
            case Mult:
                return "multiplication";
            case Div:
                return "division";
            default:
                perror("");
        }
    }

}


