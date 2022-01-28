export
OI_DIR := /home/eurosys22/OPEC/
TOP_DIR := ${OI_DIR}/experiments/
PY=python
SH=bash

.PHONY: all clean

CUR_DIR := `pwd`

all: figure11

figure11:
	${SH} ./fetch_policies.sh
	${SH} ./fetch_maps.sh
	mkdir -p ./traces
	${PY} ./generate_et_results.py -d ${CUR_DIR}
	${PY} figure11.py -d ${CUR_DIR}

clean:
	rm -r ./traces
	rm ./*.json