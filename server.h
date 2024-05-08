
#include <mutex>
#include <shared_mutex>
#include <fstream>
#include <iostream>

class log_handler {
    std::mutex m_log;
public:
    template <typename T>
    void write(T str) {
        std::lock_guard<std::mutex> l(m_log);
        std::ofstream s("log.txt", std::ios::app);
        if(!s.is_open()) {
            std::cerr << __FUNCTION__ << ": error open log file" << std::endl;
            return;
        }
        s << str << std::endl;
        s.close();
    }
};

class server {
public:
    server(unsigned port);
    bool init();
    void run();
    void exit() { active = false; }
private:
    bool active = true;
    unsigned portno;
    int sockfd = -1;

    std::shared_mutex m_clients;
    log_handler log;

    void handle_client(int newsockfd, unsigned id);
};
