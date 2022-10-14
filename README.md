# DumpAnalyzer

![C++ >=1.12.4](https://img.shields.io/badge/C++-≥_1.12.4-00599C?logo=Cplusplus)

DumpAnalyzer1.0

Written in C++ using the ROOT framework.

## Features DumpAnayzer ![Version 1.0](https://img.shields.io/badge/Version-1.0-brightgreen)
* Analyze memory dump to find sensible data by iterating over consecutives 32-byte blocks
* Print "entropy.png" graph to show where sensible data are located inside of RAM
* Simulate process of bits decay

## Features DumpAnalyzer ![Version 2.0](https://img.shields.io/badge/Version-2.0-brightgreen)
* Analyze memory dump to find sensible data by iterating over consecutive 32-byte blocks with a 1-byte sliding window
* Print "entropy.png" graph to show where sensible data are located inside of RAM
* Simulate process of bits decay

## Requirements

* C++
* ROOT

## Installation and Usage

1. Install [ROOT](https://root.cern/install/)
2. Compile Makefile
3. Run the project with ./DumpAnalyzer1 [OPTIONS] [MEMORY-DUMP.bin] [OUTPUT.csv]
4. To show help menu run ./DumpaAnalyzer1 -h

## Known  Issues
1. DumpAnalyzer 2.0 gets some issues with big dimension dump: "Fatal error: glibc detected an invalid stdio handle."

## Developers
* [Graglia Federica](https://github.com/federicaGraglia)

