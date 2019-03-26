#include <iostream>
#include <tuple>
#include <vector>
#include <string>
#include "Client.h"

using namespace std;

static const calc_t MAX_N = 50;

tuple<string, uint16_t> read_addres() 
{
    string host_name;
    uint16_t port;

    cout << "Enter host name and port" << endl;
    cin >> host_name >> port;

    return make_tuple(host_name, port);
}

void print_commands()
{
    cout << endl;
    cout << "List of commands:" << endl;
    cout << "1. get_max_prime" << endl;
    cout << "2. get_last_n <n>" << endl;
    cout << "3. calculate <n>" << endl;
    cout << "4. exit" << endl;
    cout << endl;
}

string read_command()
{
    cout << endl;
    cout << "Enter command" << endl;
    string command;
    cin >> command;
    return command;
}

int main() 
{
    try {
        string host_name;
        uint16_t port;
        tie(host_name, port) = read_addres();
        Client client(host_name, port);

        print_commands();

        while (true) 
        {
            string command = read_command();
            if (command == "get_max_prime") {
                calc_t x = client.get_max_prime();
                cout << x << endl;
            } else if (command == "get_last_n") {
                calc_t n;
                cin >> n;
                if (n > MAX_N) {
                    cout << "n should not be more than " << MAX_N << endl;
                    continue;
                }
                vector<calc_t> v = client.get_last_n(n);
                for (calc_t x : v)
                    cout << x << " ";
                cout << endl;
            } else if (command == "calculate") {
                calc_t n;
                cin >> n;
                if (n > MAX_N) {
                    cout << "n should not be more than " << MAX_N << endl;
                    continue;
                }
                client.calculate(n);
                cout << "Calculations completed" << endl;
            } else if (command == "exit") {
                break;
            } else {
                cout << "This is an unknown command" << endl;
            }
        }
    } catch (NetworkException &e) {
        cout << e.get_message() << endl;
    }

    return 0;
}