# Falcon Data Replay

This is a simple tool to replay data from [Falcon](https://github.com/falkenber9/falcon) using UDP packets

The data format is described in the [Falcon documentation](https://github.com/falkenber9/falcon). 
For convenience, the data format is also described here.

```python
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
    'format',       # index+1 of DCI format in array flacon_ue_all_formats[], see DCISearch.cc
    'ndi',          # new data indicator for first transport block
    'ndi_1',        # new data indicator for second transport block
    'harq_idx',     # HARQ index
    'ncce',         # index of first Control Channel Element (CCE) of this DCI within PDCCH
    'L',            # aggregation level of this DCI {0..3}, occupies 2^L consecutive CCEs.
    'cfi',          # number of OFDM symbols occupied by PDCCH
    'histval',      # number of occurences of this RNTI within last 200ms
    'nof_bits',     # DCI length (without CRC)
    'hex'           # raw DCI content as hex string, see sscan_hex()/sprint_hex() in falcon_dci.c 
]
```

## Design

For every UE that needs to replay the data, 
we first extract unique rnti from the data file.
The sender picks a random UE and replay the sequence of TBS.

## Requirements

- CMake >= 3.19

## Installation

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
# replay data from file
./falcon_replay --csv_path ../data/dci_20221027_1243_EDI.csv --ip 127.0.0.1 --port 5000 --count 100
./simple_udp_server --ip 127.0.0.1 --port 5000
```