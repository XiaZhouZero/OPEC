export
TOP_DIR := ${OI_DIR}/experiments/
PY=python
SH=bash

.PHONY: all clean

CUR_DIR := `pwd`

all: figure10

figure10:
	${SH} ./fetch_policies.sh
	${PY} generate_pt_results.py -d ${CUR_DIR}
	${PY} figure10.py -d ${CUR_DIR}

clean:
	rm *.json
	rm *.pdf