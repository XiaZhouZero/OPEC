# OPEC Experiments
```
.
├── table1
├── figure9
├── figure10
├── figure11
├── table2
└── table3
```

## Preparations
set `OI_DIR` environment variable. For example, 
```bash
export OI_DIR=/home/opec/OPEC/
```
Then 

## Table 1 (Security Evaluation)
Run the following commands, which should take under 1 minute to print Table 1:
```bash
OPEC/experiments$ cd table1 && make && cd -
```

## Figure 9 (Performance Evaluation)
Run the following commands, which should take under 1 minute to produce Figure 9 (Figure9_Performance_Overhead.pdf):
```bash
OPEC/experiments$ cd figure9 && make && cd -
```
>The runtime overhead results of OPEC are stored in `experiments/figure9` directory.

## Figure 10 (Partition-time Over-privilege Evaluation)
Run the following commands, which should take under 1 minute to produce Figure 10(Figure10_Partition_Overprivilege.pdf):
```bash
OPEC/experiments$ cd figure10 && make && cd -
```

## Figure 11 (Execution-time Over-privilege Evaluation)
Run the following commands, which should take under 1 minute to produce Figure 11(Figure11_Execution_Overprivilege.pdf):
```bash
OPEC/experiments$ cd figure11 && make && cd -
```

### Table 2 (Performance Comparison to ACES)
Run the following commands, which should take under 1 minute to print Table 2. *Print Table 2 before producing Figure 9*. 
```bash
OPEC/experiments$ cd table2 && make && cd -
```
>The runtime overhead results of ACES are acquired from ACES [paper](https://www.usenix.org/conference/usenixsecurity18/presentation/clements).

### Table 3 (Indirect Function Call Analysis Efficiency Evaluation)
Run the following commands, which should take under 1 minute to print Table 3:
```bash
OPEC/experiments$ cd table3 && make && cd -
```