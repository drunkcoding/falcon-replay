// A simple udp client, which can be used to test the udp server.

#include <arpa/inet.h>
#include <gflags/gflags.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <random>

DEFINE_uint32(port, 5000, "Port to connect to.");
DEFINE_string(ip, "127.0.0.1", "IP address to connect on.");
DEFINE_uint32(count, 100, "Number of messages to send.");
DEFINE_uint32(interval, 1000, "Interval between messages in ms.");

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
  if (connect(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    std::cerr << "Error connecting to server" << std::endl;
    exit(1);
  }

  char buffer[1024];
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(10, 1024);
  std::uniform_int_distribution<> dis_sleep(
      FLAGS_interval / 10, FLAGS_interval);
  for (int i = 0; i < FLAGS_count; i++) {
    // send random length message
    int n = send(socketfd, buffer, dis(gen), 0);
    if (n < 0) {
      std::cerr << "Error sending data" << std::endl;
      exit(1);
    }
    std::cout << "Sent " << n << " bytes" << std::endl;
    usleep(dis(gen) * 1000);
  }
}