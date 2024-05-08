#include "server.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <string.h>
#include <thread>
#include <iostream>

server::server(unsigned port): portno(port) {}

// #define LOG(x) x;
#define LOG(x)

#define error(msg)                                  \
{                                                   \
    std::cerr << "error: function " << __FUNCTION__ \
    << ", " << __LINE__ << std::endl;               \
    perror(msg);                                    \
}

bool server::init() {
    //create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      error("ERROR opening socket");
      return false;
    }

    //bind socket to address
    sockaddr_in serv_addr;
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port        = htons(portno);
    auto n = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (n < 0) {
      error("ERROR on binding");
      return false;
    }

    return true;
}

void server::run() {
    // inform os about socket listen
    listen(sockfd, 5);

    std::cout << "server run" << std::endl;
    while(active)
    {
        // accept connection (block wait for connection)
        LOG(std::cout << __FUNCTION__ << ": accept (wait for connection)" << std::endl)
        sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int newsockfd = accept(sockfd,(sockaddr *)&cli_addr, &clilen);
        if (newsockfd < 0) error("ERROR on accept");

        //run new thead to handle connection
        static unsigned msg_id = 0;
        msg_id++;
        LOG(std::cout << __FUNCTION__ << ": accept client " << msg_id << std::endl)
        std::thread c(&server::handle_client, this, newsockfd, msg_id);
        c.detach();
    }

    //wait for all client threads
    LOG(std::cout << __FUNCTION__ << ": wait for client threads" << std::endl)
    std::unique_lock lock(m_clients);
    LOG(std::cout << __FUNCTION__ << ": exit" << std::endl)
    close(sockfd);
}

void server::handle_client(int newsockfd, unsigned msg_id) {
    std::shared_lock l(m_clients);

    //read
    LOG(std::cout << __FUNCTION__ << " " << msg_id << ": read (wait for data)" << std::endl)
    std::string buffer(256, '\0');
    int n = read(newsockfd,(void*)buffer.data(),255);
    if (n < 0) {
      error("ERROR reading from socket");
      close(newsockfd);
      return;
    }
    std::cout << __FUNCTION__ << " " << msg_id << ": receive - " << buffer << std::endl;

    LOG(std::cout << __FUNCTION__ << " " << msg_id << ": close connection" << std::endl)
    close(newsockfd);

    log.write(buffer);
}
