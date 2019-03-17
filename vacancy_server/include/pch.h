#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <csignal>
#include <functional>
#include <cstdio>
#include <cstdlib>
#include <set>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

#include <json.hpp>

#include <atomic>
#include <tbb/concurrent_unordered_map.h>
#include <thread>

#define INT_SIZE 4
#define POWER_OF_TWO 8
//#define UCHAR_MAX 0xffu

#define INVALID_SOCKET -1

#define JSON_NAME(name, body) const char * const name = #body;

JSON_NAME(REQUEST_TYPE, requestType)
JSON_NAME(SPECIALITY, speciality)
JSON_NAME(SPECIALITY_ID, specialityId)
JSON_NAME(SUCCESS, success)
JSON_NAME(ID, id)
JSON_NAME(VACANCY, vacancy)
JSON_NAME(SPECIALITIES, specialities)
JSON_NAME(COMPANY, company)
JSON_NAME(POSITION, position)
JSON_NAME(MIN_AGE, minAge)
JSON_NAME(MAX_AGE, maxAge)
JSON_NAME(SALARY, salary)
JSON_NAME(AGE, age)
JSON_NAME(VACANCIES, vacancies)
JSON_NAME(CAUSE, cause)