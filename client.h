
#include <string.h>

class client_t
{
  unsigned timeout;
  const char *name;
  unsigned port;
  
  bool create(int & sockfd);
  bool connect(int sockfd);
  bool write(int sockfd);
  const std::string get_data();
  void disconnect(int sockfd);

public:
  client_t(const char *hostname, int portno, unsigned sec);
  void run();
};
