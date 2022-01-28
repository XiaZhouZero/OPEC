export
OI_DIR := /home/eurosys22/OPEC/
TOP_DIR := ${OI_DIR}/experiments/
PY=python
SH=bash


.PHONY: all clean

all:
	${SH} ./fetch_OPEC_policy.sh
	${PY} ./table1.py

clean:
	rm ./*.json