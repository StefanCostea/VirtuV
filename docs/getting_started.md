# Getting Started

Welcome to the VirtuV documentation! This guide will help you set up and start using the RISC-V emulator platform.

## Prerequisites

- A compatible C++ compiler supporting C++23
- CMake (version 3.26 or later)
- Python3 and Pybind11 (for tests and Python bindings)
- Boost

## Quick Start

Before building the project, make sure to set the required environment variables:

```bash
source setup.env
```

Generate Build Files with CMake
In the project root, run:
```bash
cmake .
```
> **_NOTE:_** If you change compile options or update your environment variables, run:
>```bash
>cmake . --fresh
>```
> **This ensures that the new settings are applied.**

Then build the project with:
In the project root, run:
```bash
make -j12
```

## Running tests
VirtuV includes a suite of tests written in Python. Once the build is complete, run:
```bash
make test
```
