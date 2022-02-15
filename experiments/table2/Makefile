export
TOP_DIR := ${OI_DIR}/experiments/
PY=python
SH=bash

.PHONY: all clean

CUR_DIR := `pwd`

all: table2

table2:
	# ${SH} ./fetch_ACES_bins.sh
	${SH} ./fetch_map_files.sh
	${PY} ./evaluate_ACES_memory_overhead.py -d ${CUR_DIR}
	${SH} ./fetch_OPEC_flash_sram_overhead.sh ${CUR_DIR}
	${PY} ./table2.py -d ${CUR_DIR}

clean:
	rm ./*.json