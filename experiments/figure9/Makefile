export
TOP_DIR := ${OI_DIR}/experiments/
PY=python
SH=bash

.PHONY: all clean

CUR_DIR := `pwd`

all: figure9

figure9:
	${SH} ./fetch_binary.sh
	${PY} memory_overhead.py -d ${CUR_DIR}
	${PY} figure9.py -d ${CUR_DIR}

clean:
	rm *.json
	rm *.pdf