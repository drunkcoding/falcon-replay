#include <rapidcsv.h>
#include <iostream>

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

class CSVParser {
 public:
  explicit CSVParser(std::string path)
      : doc_(path, rapidcsv::LabelParams(0, -1), rapidcsv::SeparatorParams('\t'))
  {
    // print doc_
    auto column_names = doc_.GetColumnNames();
    for (int i = 0; i < column_names.size(); i++) {
      std::cout << column_names[i] << std::endl;
    }
    // SetColumnName to the column_names_ vector
    for (int i = 0; i< column_names_.size(); i++) {
      doc_.SetColumnName(i, column_names_[i]);
    }
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
        "timestamp", "sfn", "subframe", "rnti", "direction", "mcs_idx", "nof_prb",
        "tbs_sum",   "tbs_0", "tbs_1",   "format", "ndi",       "ndi_1",
        "harq_idx",  "ncce",  "L",       "cfi",    "histval",   "nof_bits",
        "hex"
  };
};