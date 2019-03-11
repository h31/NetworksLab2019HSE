#include "../include/value.h"
#include "../include/no_value_exception.h"


void value::update(int value) {
    pthread_mutex_lock(&value_access);
    while (has_value) {
        pthread_cond_wait(&value_queried, &value_access);
    }
    this->value = value;
    has_value = true;
    pthread_cond_signal(&value_updated);
    pthread_mutex_unlock(&value_access);
}

int value::get() {
    check_is_closed();
    pthread_mutex_lock(&value_access);
    while (!has_value) {
        pthread_cond_wait(&value_updated, &value_access);
        check_is_closed();
    }
    int value = this->value;
    has_value = false;
    pthread_cond_signal(&value_queried);
    pthread_mutex_unlock(&value_access);
    return value;
}

void value::close() {
    is_closed = true;
    pthread_cond_broadcast(&value_updated);
}

void value::check_is_closed() {
    if (is_closed) {
        throw no_value_exception();
    }
}
