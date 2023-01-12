import os
import pandas as pd
import numpy as np
import argparse
from tqdm import tqdm

COLUMNS_FALCON_DCI = [
    "timestamp",  # unix timestamp in [s] as float, 1µs resolution
    "sfn",  # system frame number
    "subframe",  # subframe index {0,1,...,9}
    "rnti",  # the addressed RNTI
    "direction",  # 0 for uplink alloc., 1 for downlink alloc.
    "mcs_idx",  # MCS index of the first transport block
    "nof_prb",  # number of allocated PRBs
    "tbs_sum",  # total Transport Block Size (TBS) in [Bit]
    "tbs_0",  # TBS of first transport block (-1 for SISO)
    "tbs_1",  # TBS of second transport block (-1 for SISO)
    "format",  # index+1 of DCI format in array flacon_ue_all_formats[], see DCISearch.cc
    "ndi",  # new data indicator for first transport block
    "ndi_1",  # new data indicator for second transport block
    "harq_idx",  # HARQ index
    "ncce",  # index of first Control Channel Element (CCE) of this DCI within PDCCH
    "L",  # aggregation level of this DCI {0..3}, occupies 2^L consecutive CCEs.
    "cfi",  # number of OFDM symbols occupied by PDCCH
    "histval",  # number of occurences of this RNTI within last 200ms
    "nof_bits",  # DCI length (without CRC)
    "hex",  # raw DCI content as hex string, see sscan_hex()/sprint_hex() in falcon_dci.c
]

args = argparse.ArgumentParser()
args.add_argument("--input", type=str, required=True)

args = args.parse_args()

df = pd.read_csv(args.input, sep="\t", names=COLUMNS_FALCON_DCI)

# only keep timestamp to tbs_sum column

df = df.iloc[:, 0:8]

dirname = os.path.dirname(args.input)
df.to_csv(os.path.join(dirname, "dci.csv"), sep=",", index=False, header=True)

df_grouped = df.groupby("rnti")
for group in tqdm(df_grouped):
    df_rnti = group[1]
    df_rnti = df_rnti.reset_index()
    df_rnti.to_csv(
        os.path.join(dirname, "rnti", "dci_{}.csv".format(group[0])),
        sep=",",
        index=False,
        header=True,
    )
