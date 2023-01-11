// A simple udp server, which can be used to test the udp sender.

#include <arpa/inet.h>
#include <gflags/gflags.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <iostream>

DEFINE_uint32(port, 5000, "Port to listen on.");
DEFINE_string(ip, "0.0.0.0", "IP address to listen on.");

int
main(int argc, char** argv)
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (socketfd < 0) {
    std::cerr << "Error creating socket" << std::endl;
    exit(1);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(FLAGS_port);
  server_addr.sin_addr.s_addr = inet_addr(FLAGS_ip.c_str());
  if (bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    std::cerr << "Error binding to server" << std::endl;
    exit(1);
  }

  char buffer[1024];
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  while (true) {
    int n = recvfrom(
        socketfd, buffer, 1024, 0, (struct sockaddr*)&client_addr,
        &client_addr_len);
    if (n < 0) {
      std::cerr << "Error receiving data" << std::endl;
      exit(1);
    }
    std::cout << "Received " << n << " bytes" << std::endl;
  }
}