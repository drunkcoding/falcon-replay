

#include <gflags/gflags.h>

#include <chrono>
#include <filesystem>
#include <thread>

#include "csv_parser.h"
#include "udp_sender.h"

DEFINE_string(
    csv_path, "", "Path to the CSV file containing the data to be replayed.");
DEFINE_uint32(port, 5000, "Port to send the data to.");
DEFINE_string(ip, "127.0.0.1", "Target IP address to send the data to.");
DEFINE_uint32(count, 100, "Number of rnti to replay.");

int
main(int argc, char** argv)
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  // ...
  CSVParser parser(FLAGS_csv_path);
  UDPSender sender(FLAGS_ip.c_str(), FLAGS_port);

  auto rnti = parser.GetUniqueRNTI();

  std::uint32_t rnti_replayed = 0;
  while (rnti_replayed < FLAGS_count) {
    // pick a random rnti
    auto r = rnti[rand() % rnti.size()];
    auto tbs = parser.GetTBSByRNTI(r);
    auto timestamp = parser.GetTimestampByRNTI(r);

    for (int i = 0; i < tbs.size(); i++) {
      // send the data
      sender.SendFakeData(tbs[i]);
      // sleep for the time difference between the current and the next
      // timestamp
      if (i < tbs.size() - 1) {
        auto time_diff = timestamp[i + 1] - timestamp[i];
        if (time_diff > 10000) {
          time_diff = 1000;
        }
        std::cout << "Sended " << tbs[i] << " bytes; Sleeping for " << time_diff << " ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_diff));
      }
    }
    // send the data
    rnti_replayed++;
  }
}