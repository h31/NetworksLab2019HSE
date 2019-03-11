#ifndef UNTITLED_CONSUMER_PARAMETERS_H
#define UNTITLED_CONSUMER_PARAMETERS_H

struct consumer_parameters {
        const int* value;
        const long max_sleep_time;
        const long consumers_number;

        consumer_parameters(const int *value, long max_sleep_time, long consumers_number)
            : value(value), max_sleep_time(max_sleep_time), consumers_number(consumers_number) {}
};

#endif //UNTITLED_CONSUMER_PARAMETERS_H
