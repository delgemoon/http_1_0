#include <iostream>
#include <server.hpp>
#include <signal.h>
#include <util.hpp>
std::atomic<bool> stop = false;
http10::server::server* s_ptr = nullptr;
using namespace std::string_literals;
using namespace std::chrono_literals;
static void signal_handler(int sig)
{
    std::cout << "signal handler\n" << sig << std::endl;
    std::this_thread::sleep_for(1s);
}
int main(int argc, char* argv[]) {
    try {
        if (argc != 5)
        {
            std::cerr << "Usage: dlt";
            std::cerr << " <listen_address> <num thread> <detached> <root director>";
            return 1;
        }
        http10::server::server s(http10::server::str_to_u16(argv[1]), http10::server::str_to_int(argv[2]), http10::server::str_to_bool(argv[3]), std::string(argv[4]) );
        //s_ptr = &s;
        struct sigaction sa;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;;
        sa.sa_handler = signal_handler;
        if (sigaction(SIGINT, &sa, NULL) == -1)
            return -1;
        s.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "other Exception: " << "\n";
    }
    return 0;
}
