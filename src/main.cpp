#include "App.h"
#include <iostream>

int main() {
    uWS::App().get("/*", [](auto *res, auto */*req*/) {
        res->end("Hello world!");
    }).listen(3001, [](auto *listen_socket) {
        if (listen_socket) {
            std::cout << "Listening on port 3001" << std::endl;
        } else {
            std::cout << "Failed to listen on port 3001" << std::endl;
        }
    }).run();

    std::cout << "Event loop exited" << std::endl;
}