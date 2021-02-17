#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <aether/common/tcp.hh>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace::std::literals;

int main() {
    size_t iters = 1'000'000;

    using time = std::chrono::high_resolution_clock;
    auto t0 = time::now();
    std::string mode = "--tcp";
    if (mode == "--atomic") {
        std::atomic<bool> flag {};

        std::jthread a([iters, &flag]{
            for (size_t counter = 0; counter < iters; counter++) {
                bool swap = false;
                while (!flag.compare_exchange_weak(swap, true)) {}
            }
        });

        std::jthread b([iters, &flag]{
            for (size_t counter = 0; counter < iters; counter++){
                bool swap = true;
                while (!flag.compare_exchange_weak(swap, false)) {}
            }
        });
    } else if (mode == "--tcp") {
        std::jthread a([iters]{
            aether::tcp::os_socket s = aether::tcp::connect_to_host_port_with_timeout("127.0.0.1", "9999", 10);
            (void)s;
            bool a_flag = true;
            for (size_t counter = 0; counter < iters; counter++){
                if (counter & 1) {
                    send(s, &a_flag, sizeof(a_flag), 0);
                } else {
                    recv(s, &a_flag, sizeof(a_flag), 0);
                }
            }
        });
        std::jthread b([iters]{
            int sock = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in addr;
            if (!inet_aton("127.0.0.1", &addr.sin_addr)) {
                std::cerr << "invalid address" << std::endl;
                abort();
            }
            addr.sin_port = htons(atoi("9999"));
            addr.sin_family = AF_INET;
            struct sockaddr* saddr = reinterpret_cast<struct sockaddr*>(&addr);
            if (bind(sock, saddr, sizeof(*saddr))) {
                perror("bind");
                abort();
            }
            if (listen(sock, -42)) {
                perror("listen");
                abort();
            }
            int s = accept(sock, NULL, 0);
            bool b_flag = true;
            for (size_t counter = 0; counter < iters; counter++){
                if (counter & 1) {
                    recv(s, &b_flag, sizeof(b_flag), 0);
                } else {
                    send(s, &b_flag, sizeof(b_flag), 0);
                }
            }
        });
    }
    auto t1 = time::now();
    std::chrono::duration<double> duration = t1 - t0;
    std::cout << duration.count() << "s\n";
    std::cout << duration.count() / iters * 1e9 << "ns per iter\n";
}
