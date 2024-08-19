# Verodb - In-Memory SQL Database

## Overview

Verodb is an in-memory SQL database implemented in C, designed for educational purposes. This project includes modules for querying, parsing SQL, and managing the database structure.

## Local Setup

### 1. CMake Installations

For **Ubuntu/Debian-based** distributions:
```bash
sudo apt update
sudo apt install cmake
```
For **MacOS**:
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake
```

### 2. Fork and Clone the Repository

First, clone the repository to your local machine:

```sh
git clone https://github.com/your-username/verodb.git
cd verodb
```

After cloning you need to create a build folder where all the builds of the monorepo will be saved.

```sh
mkdir build
cd build
```

Generate Build Files with CMake

```sh
cmake ..
```

Compile Project

```sh
make
```

Run the executable file

```sh
engine/verodb
```
