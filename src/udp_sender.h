#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

class UDPSender {
 public:
  explicit UDPSender(const char* ip, int port)
  {
    socketfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd_ < 0) {
      std::cerr << "Error creating socket" << std::endl;
      exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);
    if (connect(
            socketfd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
        0) {
      std::cerr << "Error connecting to server" << std::endl;
      exit(1);
    }
  }
  ~UDPSender() { close(socketfd_); }

  void Send(const char* data, int size)
  {
    if (send(socketfd_, data, size, 0) < 0) {
      std::cerr << "Error sending data" << std::endl;
      exit(1);
    }
  }

  void Send(const std::string& data) { Send(data.c_str(), data.size()); }

  void SendFakeData(int size)
  {
    std::string data(size, 'a');
    Send(data);
  }

 private:
  int socketfd_;
};