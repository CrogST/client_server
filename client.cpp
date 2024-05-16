#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <thread>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "client.h"

// #define LOG(x) x;
#define LOG(x)

#define error(msg)                                  \
{                                                   \
    std::cerr << "error: function " << __FUNCTION__ \
    << ", " << __LINE__ << std::endl;               \
    perror(msg);                                    \
}

client_t::client_t(const char *_name, int portno, unsigned sec):
    name(_name), port(portno), timeout(sec)
{}

void client_t::run()
{
    bool first = true;
    std::cout << "client " << name << " run" << std::endl;
    while (true)
    {
        // wait
        if(!first) std::this_thread::sleep_for(std::chrono::seconds(timeout));
        else first = false;

        int sockfd = 0;
        if(!create(sockfd)) {
            error("ERROR opening socket");
            continue;
        }

        if (!connect(sockfd)) {
            error("ERROR connect");
            continue;
        }
        if (!write(sockfd)) {
            std::cerr << "send message failed" << std::endl;
            continue;
        }
    }
}

bool client_t::create(int & sockfd) {
    LOG(std::cout << __FUNCTION__ << ": open socket" << std::endl)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    return sockfd >= 0;
}

bool client_t::connect(int sockfd)
{
    // connect
    LOG(std::cout << __FUNCTION__ << ": connect" << std::endl)
    hostent *server = gethostbyname("localhost");
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        ::exit(0);
    }
    sockaddr_in serv_addr;
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);

    if (::connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
        return false;
    }
    LOG(std::cout << __FUNCTION__ << ": connect ok" << std::endl)
    return true;
}

const std::string client_t::get_data() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = now.time_since_epoch().count() % 1000;

    std::stringstream ss;
    ss << '[';
    ss << std::put_time(std::localtime(&in_time_t), "%F %T.");
    ss << std::setfill('0') << std::setw(3) << ms << "] ";
    ss << name;
    return ss.str();
}

bool client_t::write(int sockfd)
{
    auto str = get_data();

    std::cout << __FUNCTION__ << ": send - '" << str << "'" << std::endl;

    auto n = ::write(sockfd, reinterpret_cast<const void*>(str.data()), str.length());
    if (n < 0) {
        error("ERROR writing to socket");
        return false;
    }
    return true;
}

void client_t::disconnect(int sockfd)
{
    LOG(std::cout << __FUNCTION__ << ": close socket" << std::endl)
    close(sockfd);
    LOG(std::cout << __FUNCTION__ << ": exit" << std::endl)
}
