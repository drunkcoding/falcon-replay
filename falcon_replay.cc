

#include <gflags/gflags.h>

#include <chrono>
#include <filesystem>
#include <fstream>
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

  // List all csv files under FLAGS_csv_path
  std::vector<std::string> csv_files;
  for (const auto& entry : std::filesystem::directory_iterator(FLAGS_csv_path)) {
    csv_files.push_back(entry.path());
  }
  std::cout << "Found " << csv_files.size() << " unique rnti" << std::endl;
  std::cout << "First file " <<  csv_files[0] << std::endl;

  UDPSender sender(FLAGS_ip.c_str(), FLAGS_port);

  std::uint32_t rnti_replayed = 0;
  while (rnti_replayed < FLAGS_count) {
    // pick a random rnti
    auto r = csv_files[rand() % csv_files.size()];
    CSVParser parser(r);
    auto tbs_str = parser.GetColumn("tbs_sum");
    auto timestamp_str = parser.GetColumn("timestamp");

    std::vector<std::uint32_t> tbs;
    std::vector<std::uint32_t> timestamp;

    for (auto& s : tbs_str) {
      tbs.push_back(std::stoul(s) / 8);
    }
    for (auto& s : timestamp_str) {
      timestamp.push_back(std::stod(s) * 1000);
    }

    std::cout << "Replaying " << tbs.size() << " packets for rnti " << r
              << std::endl;

    for (int i = 0; i < tbs.size(); i++) {
      // send the data
      sender.SendFakeData(tbs[i] + 1);
      // sleep for the time difference between the current and the next
      // timestamp
      if (i < tbs.size() - 1) {
        auto time_diff = timestamp[i + 1] - timestamp[i];
        if (time_diff > 10000) {
          time_diff = 1000;
        }
        std::cout << "Sended " << tbs[i] << " bytes; Sleeping for " << time_diff
                  << " ms" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(time_diff));
      }
    }
    // send the data
    rnti_replayed++;
  }
}