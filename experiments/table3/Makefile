export
TOP_DIR := ${OI_DIR}/experiments/
PY=python
SH=bash

.PHONY: all clean

all: point_to_eval

point_to_eval:
	${SH} ./fetch_point_to_results.sh
	${PY} ./table3.py

clean:
	rm -r ./raw_results
	rm ./*.json