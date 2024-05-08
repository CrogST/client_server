#include <iostream>

#include "server.h"

static const bool _debug = false;

/*
сервер получает из командной строки номер порта
    слушает порт
    получает сообщения от клиентов
    пишет сообщения в файл log.txt
    (каждый клиент обрабатывается в отд. потоке
     каждое сообщение пишется с новой строки)
клиент: получает 3 аргумента.
        имя клиента,
        номер порта
        интервал обмена в секундах
    С заданным интервало клиент подключается к серверу и оправляет:
        [yyyy-mm-dd hh:mm:ss.ms] client_name
*/

auto parse_args(auto argc, auto argv) {
    if(argc < 2) {
        std::cerr << "server <port number>" << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned portno;
    try {
        portno = std::stoi(argv[1]);
    } catch(...) {
        std::cerr << "server <port number>" << std::endl;
        exit(EXIT_FAILURE);
    }
    return portno;
}

int main(int argc, char * argv[]) {
    unsigned portno;
    if(_debug) {
      std::cout << "debug mode!" << std::endl;
      portno = 10005;
    } else {
      portno = parse_args(argc, argv);
    }

    server s(portno);
    if(!s.init()) {
      std::cerr << "server init failed" << std::endl;
      return -1;
    }
    s.run();
    return 0;
}
