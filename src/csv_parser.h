#include <rapidcsv.h>

#include <csv2/parameters.hpp>
#include <csv2/reader.hpp>
#include <csv2/writer.hpp>
#include <iostream>
#include <string>

/*
COLUMNS_FALCON_DCI = [
    'timestamp',    # unix timestamp in [s] as float, 1µs resolution
    'sfn',          # system frame number
    'subframe',     # subframe index {0,1,...,9}
    'rnti',         # the addressed RNTI
    'direction',    # 0 for uplink alloc., 1 for downlink alloc.
    'mcs_idx',      # MCS index of the first transport block
    'nof_prb',      # number of allocated PRBs
    'tbs_sum',      # total Transport Block Size (TBS) in [Bit]
    'tbs_0',        # TBS of first transport block (-1 for SISO)
    'tbs_1',        # TBS of second transport block (-1 for SISO)
    'format',       # index+1 of DCI format in array flacon_ue_all_formats[],
see DCISearch.cc 'ndi',          # new data indicator for first transport block
    'ndi_1',        # new data indicator for second transport block
    'harq_idx',     # HARQ index
    'ncce',         # index of first Control Channel Element (CCE) of this DCI
within PDCCH 'L',            # aggregation level of this DCI {0..3}, occupies
2^L consecutive CCEs. 'cfi',          # number of OFDM symbols occupied by PDCCH
    'histval',      # number of occurences of this RNTI within last 200ms
    'nof_bits',     # DCI length (without CRC)
    'hex'           # raw DCI content as hex string, see
sscan_hex()/sprint_hex() in falcon_dci.c
]
*/

std::vector<std::string>
split_string(std::string str, char delimiter)
{
  std::vector<std::string> internal;
  std::stringstream ss(str);  // Turn the string into a stream.
  std::string tok;

  while (getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

class SimpleCSVParser {
 public:
  explicit SimpleCSVParser(std::string path)
  {
    fd_ = fopen(path.c_str(), "r");
    if (fd_ < 0) {
      std::cerr << "Error opening file" << std::endl;
      exit(1);
    }

    // read the first line as column name
    char buffer[1024];
    memset(buffer, '\0', 1024);
    fscanf(fd_, "%s", buffer);
    std::string line(buffer);
    column_names_ = split_string(line, ',');
    memset(buffer, '\0', 1024);

    // read the rest of the file
    
    while (fscanf(fd_, "%s", buffer) != EOF) {
      std::string line(buffer);
      csv_.push_back(split_string(line, ','));
      memset(buffer, '\0', 1024);
    }
  }

  std::vector<std::string> GetColumn(std::string column_name) const
  {
    // find the index of column
    int column_idx = 0;
    for (int i = 0; i < column_names_.size(); i++) {
      if (column_names_[i] == column_name) {
        column_idx = i;
        break;
      }
    }

    std::cout << "Column " << column_name << " is at index " << column_idx
              << std::endl;

    std::vector<std::string> column;
    for (const auto& row : csv_) {
      int idx = 0;
      for (const auto& cell : row) {
        if (idx == column_idx) {
          column.push_back(cell);
          // std::cout << "cell value: " << value << std::endl;
        }
        idx++;
      }
    }
    return column;
  }

  std::vector<std::string> GetUniqueRNTI() const
  {
    auto rnti = GetColumn("rnti");
    std::sort(rnti.begin(), rnti.end());
    auto last = std::unique(rnti.begin(), rnti.end());
    rnti.erase(last, rnti.end());
    return rnti;
  }

  std::vector<std::uint32_t> GetTBSByRNTI(const std::string rnti)
  {
    auto rnti_column = GetColumn("rnti");
    auto tbs_column = GetColumn("tbs_sum");
    std::vector<std::uint32_t> tbs_by_rnti;
    for (int i = 0; i < rnti_column.size(); i++) {
      if (rnti_column[i] == rnti) {
        auto tbs = std::stoul(tbs_column[i]);
        tbs_by_rnti.push_back(tbs / 8);
      }
    }
    return tbs_by_rnti;
  }

  std::vector<std::uint32_t> GetTimestampByRNTI(const std::string rnti)
  {
    auto rnti_column = GetColumn("rnti");
    auto timestamp_column = GetColumn("timestamp");
    std::vector<std::uint32_t> timestamp_by_rnti;
    for (int i = 0; i < rnti_column.size(); i++) {
      if (rnti_column[i] == rnti) {
        auto timestamp = std::stod(timestamp_column[i]);
        uint32_t millis = timestamp * 1000;
        timestamp_by_rnti.push_back(millis);
      }
    }
    return timestamp_by_rnti;
  }


 private:
  FILE* fd_;
  std::vector<std::vector<std::string>> csv_;
  std::vector<std::vector<std::string>> columns_;
  std::vector<std::string> column_names_;
};

class ModernCSVParser {
 public:
  // explicit ModernCSVParser(std::string path) { csv_.mmap(path.c_str()); }
  explicit ModernCSVParser(std::string content) { csv_.parse(content); }

  std::vector<std::string> GetColumn(std::string column_name) const
  {
    // find the index of column
    int column_idx = 0;
    for (int i = 0; i < column_names_.size(); i++) {
      if (column_names_[i] == column_name) {
        column_idx = i;
        break;
      }
    }

    std::cout << "Column " << column_name << " is at index " << column_idx
              << std::endl;

    std::vector<std::string> column;
    for (auto row = csv_.begin(); row != csv_.end(); ++row) {
      int idx = 0;
      std::string row_value;
      (*row).read_raw_value(row_value);
      std::cout << "cell value: " << row_value << std::endl;
      for (const auto cell : *row) {
        // Do something with cell value
        std::string value;
        cell.read_value(value);
        std::cout << "cell value: " << value << std::endl;
        if (idx == column_idx) {
          column.push_back(value);
          // std::cout << "cell value: " << value << std::endl;
        }
        idx++;
      }
    }
    return column;
  }

  std::vector<std::string> GetUniqueRNTI() const
  {
    auto rnti = GetColumn("rnti");
    std::sort(rnti.begin(), rnti.end());
    auto last = std::unique(rnti.begin(), rnti.end());
    rnti.erase(last, rnti.end());
    return rnti;
  }

  std::vector<std::uint32_t> GetTBSByRNTI(const std::string rnti)
  {
    auto rnti_column = GetColumn("rnti");
    auto tbs_column = GetColumn("tbs_sum");
    std::vector<std::uint32_t> tbs_by_rnti;
    for (int i = 0; i < rnti_column.size(); i++) {
      if (rnti_column[i] == rnti) {
        auto tbs = std::stoul(tbs_column[i]);
        tbs_by_rnti.push_back(tbs / 8);
      }
    }
    return tbs_by_rnti;
  }

  std::vector<std::uint32_t> GetTimestampByRNTI(const std::string rnti)
  {
    auto rnti_column = GetColumn("rnti");
    auto timestamp_column = GetColumn("timestamp");
    std::vector<std::uint32_t> timestamp_by_rnti;
    for (int i = 0; i < rnti_column.size(); i++) {
      if (rnti_column[i] == rnti) {
        auto timestamp = std::stod(timestamp_column[i]);
        uint32_t millis = timestamp * 1000;
        timestamp_by_rnti.push_back(millis);
      }
    }
    return timestamp_by_rnti;
  }

 private:
  csv2::Reader<
      csv2::delimiter<','>, csv2::quote_character<'"'>,
      csv2::first_row_is_header<true>, csv2::trim_policy::trim_whitespace>
      csv_;
  std::vector<std::string> column_names_{
      "timestamp", "sfn",     "subframe", "rnti",     "direction",
      "mcs_idx",   "nof_prb", "tbs_sum",  "tbs_0",    "tbs_1",
      "format",    "ndi",     "ndi_1",    "harq_idx", "ncce",
      "L",         "cfi",     "histval",  "nof_bits", "hex"};
};

class CSVParser {
 public:
  explicit CSVParser(std::string path)
      : doc_(
            path, rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams(','))
  {
    // print doc_
    column_names_ = doc_.GetColumnNames();
    // for (int i = 0; i < column_names.size(); i++) {
    //   std::cout << column_names[i] << std::endl;
    // }
    // // SetColumnName to the column_names_ vector
    // for (int i = 0; i < column_names_.size(); i++) {
    //   doc_.SetColumnName(i, column_names_[i]);
    // }
  }

  std::vector<std::string> GetColumnNames() const { return column_names_; }

  std::vector<std::string> GetColumn(std::string column_name) const
  {
    return doc_.GetColumn<std::string>(column_name);
  }

  std::vector<std::string> GetRow(int row) const
  {
    return doc_.GetRow<std::string>(row);
  }

  std::string GetCell(int row, std::string column_name) const
  {
    return doc_.GetCell<std::string>(row, column_name);
  }

  std::vector<std::string> GetUniqueRNTI() const
  {
    auto rnti = GetColumn("rnti");
    std::sort(rnti.begin(), rnti.end());
    auto last = std::unique(rnti.begin(), rnti.end());
    rnti.erase(last, rnti.end());
    return rnti;
  }

  std::vector<std::uint32_t> GetTBSByRNTI(const std::string rnti)
  {
    auto rnti_column = GetColumn("rnti");
    auto tbs_column = GetColumn("tbs_sum");
    std::vector<std::uint32_t> tbs_by_rnti;
    for (int i = 0; i < rnti_column.size(); i++) {
      if (rnti_column[i] == rnti) {
        auto tbs = std::stoul(tbs_column[i]);
        tbs_by_rnti.push_back(tbs / 8);
      }
    }
    return tbs_by_rnti;
  }

  std::vector<std::uint32_t> GetTimestampByRNTI(const std::string rnti)
  {
    auto rnti_column = GetColumn("rnti");
    auto timestamp_column = GetColumn("timestamp");
    std::vector<std::uint32_t> timestamp_by_rnti;
    for (int i = 0; i < rnti_column.size(); i++) {
      if (rnti_column[i] == rnti) {
        auto timestamp = std::stod(timestamp_column[i]);
        uint32_t millis = timestamp * 1000;
        timestamp_by_rnti.push_back(millis);
      }
    }
    return timestamp_by_rnti;
  }

 private:
  rapidcsv::Document doc_;
  std::vector<std::string> column_names_{
      "timestamp", "sfn",     "subframe", "rnti",     "direction",
      "mcs_idx",   "nof_prb", "tbs_sum",  "tbs_0",    "tbs_1",
      "format",    "ndi",     "ndi_1",    "harq_idx", "ncce",
      "L",         "cfi",     "histval",  "nof_bits", "hex"};
};