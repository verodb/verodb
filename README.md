# Verodb - In-Memory SQL Database

## Overview

Verodb is an in-memory SQL database implemented in C, designed for educational purposes. This project includes modules for querying, parsing SQL, and managing the database structure.

## Getting Started

### Prerequisites

Ensure you have the following installed on your system:

- **CMake** (3.0 or higher)
- **GNU Compiler Collection (GCC)** for C
- **Flex** (for lexical analysis)
- **Bison** (for parsing)

### Fork and Clone the Repository

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
