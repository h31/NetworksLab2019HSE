
#include <cstdint>
#include <cmath>
#include <unistd.h>
#include <cstdio>

#pragma once
namespace operations {

    enum Type {
        Fact = 1,
        Sq = 2,
        Add = 3,
        Subs = 4,
        Mult = 5,
        Div = 6
    };


    struct Operation {
        Operation(Type type) : type(type) {}

        virtual int32_t evaluate() const;

        virtual ~Operation() {}

        virtual Type getType() const {
            return type;
        }

    private:
        Type type;
    };

    struct UnaryOperation : Operation {
        UnaryOperation(const int32_t _argument, Type type)
                : _argument(_argument),
                  Operation(type) {}

    protected:
        const int32_t _argument;
    };

    struct BinaryOperation : Operation {
        BinaryOperation(const int32_t _first, const int32_t _second, Type type)
                : _first(_first),
                  _second(_second),
                  Operation(type) {}

    protected:
        const int32_t _first;
        const int32_t _second;
    };

    struct Factorial : UnaryOperation {
        Factorial(const int32_t &_argument) : UnaryOperation(_argument, Type::Fact) {}

        int32_t evaluate() const override {
            if (_argument < 0 || _argument > 10) {
                throw "TODO";
            }

            int32_t result = 1;
            for (int32_t i = _argument; i > 0; --i) {
                result *= i;
            }

            return result;
        }
    };

    struct LongOperation : Operation {
        LongOperation(const Operation *op, const int32_t ms) : op(op), ms(ms), Operation(op->getType()) {}

        int32_t evaluate() const override {
            sleep(ms);
            return op->evaluate();
        }


        ~LongOperation() override {
            delete op;
        }

    private:
        const Operation *op;
        const uint32_t ms;
    };

    struct Square : UnaryOperation {
        Square(const int32_t &_argument) : UnaryOperation(_argument, Type::Sq) {}

        int32_t evaluate() const override {
            return static_cast<int32_t>(std::pow(_argument, 2.0));
        }
    };

    struct Addition : BinaryOperation {
        Addition(const int32_t _first, const int32_t _second) : BinaryOperation(_first, _second, Type::Add) {}

        int32_t evaluate() const override {
            return _first + _second;
        }
    };

    struct Substraction : BinaryOperation {
        Substraction(const int32_t _first, const int32_t _second) : BinaryOperation(_first, _second,
                                                                                    Type::Subs) {}

        int32_t evaluate() const override {
            return _first - _second;
        }
    };

    struct Multiplication : BinaryOperation {
        Multiplication(const int32_t _first, const int32_t _second) : BinaryOperation(_first, _second,
                                                                                      Type::Mult) {}

        int32_t evaluate() const override {
            return _first * _second;
        }
    };

    struct Division : BinaryOperation {
        Division(const int32_t _first, const int32_t _second) : BinaryOperation(_first, _second, Type::Div) {}

        int32_t evaluate() const override {
            return _first / _second;
        }
    };

    Operation *createOperation(char &type, int32_t arg1, int32_t arg2) {
        switch (type) {
            case Type::Fact :
                return new operations::LongOperation(new Factorial(arg1), arg2);
            case Type::Sq:
                return new operations::LongOperation(new Square(arg1), arg2);
            case Type::Add:
                return new operations::Addition(arg1, arg2);
            case Subs:
                return new operations::Substraction(arg1, arg2);
            case Mult:
                return new operations::Multiplication(arg1, arg2);
            case Div:
                return new operations::Division(arg1, arg2);
            default:
                perror("");
        }
    }
}

