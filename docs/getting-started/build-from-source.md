# Building from Source

In actual projects, we usually need to build Skynet from source code, which allows for easier debugging and customization. Here we document the experience from the build process.

## System Requirements

### Linux

- GCC or Clang
- GNU Make
- autoconf (for jemalloc)

### macOS

- Xcode Command Line Tools
- GNU Make (gmake)

### FreeBSD

- GCC or Clang
- GNU Make (gmake)

## Getting the Source Code

```bash
git clone https://github.com/cuihairu/skynet.git
cd skynet
```

## Building Skynet

### Linux

```bash
make linux
```

Or:

```bash
export PLAT=linux
make
```

### macOS

```bash
make macosx
```

### FreeBSD

```bash
gmake freebsd
```

### OpenBSD

```bash
gmake openbsd
```

## Build Options

### Specifying the Platform

```bash
make PLATFORM
# PLATFORM can be: linux, macosx, freebsd, openbsd
```

### Cleaning the Build

```bash
make cleanall
```

### Building Lua Only

```bash
make lua
```

### Building Skynet Only

```bash
make skynet
```

## Build System Structure

### Makefile Structure

```
Makefile              # Main build file
├── platform.mk       # Platform-specific configuration
├── mingw.mk          # MinGW (Windows) configuration
└── 3rd/
    ├── lua/          # Lua build
    ├── jemalloc/     # jemalloc memory allocator
    └── lpeg/         # LPEG module
```

### Main Makefile

```makefile
# Main build targets
all : $(ALL)

# Skynet core
SKYNET_SRC = skynet-src

# Source files
SKYNET_DEFINES =
CSERVICE = cservice

# Build skynet
skynet : $(LUA_LIB) $(JEMALLOC_STATICLIB) $(SKYNET_DEFINES)
	$(CC) $(CFLAGS) -o $@ ...
```

### Platform Configuration

```makefile
# platform.mk

# Linux configuration
ifeq ($(PLAT),linux)
  CFLAGS += -DUSE_$(EXPORT_FLAG)
  SHARED := -shared
  LIBS += -lpthread -lrt
endif

# macOS configuration
ifeq ($(PLAT),macosx)
  SHARED := -bundle -undefined dynamic_lookup
endif
```

## Core Component Builds

### 1. Lua

Skynet uses a modified version of Lua 5.5.0:

```bash
# Build Lua
make lua

# Output files
3rd/lua/lua        # Lua interpreter
3rd/lua/luac       # Lua compiler
3rd/lua/liblua.a   # Lua static library
```

### 2. jemalloc

High-performance memory allocator:

```bash
# Build jemalloc
make jemalloc

# Output files
3rd/jemalloc/lib/libjemalloc.a
```

### 3. LPEG

Lua pattern matching library:

```bash
# Build LPEG
make lpeg

# Output files
luaclib/lpeg.so
```

## Service Module Builds

### C Services

C service modules are compiled into `.so` files:

```bash
# Build all C services
make cservice

# Output directory
cservice/
├── logger.so      # Logger service
├── gate.so        # Gateway service
├── harbor.so      # Cluster service
└── ...
```

### Lua C Modules

Lua C extension modules:

```bash
# Build all Lua C modules
make luaclib

# Output directory
luaclib/
├── bson.so        # BSON support
├── md5.so         # MD5 hash
├── lpeg.so        # LPEG pattern matching
└── ...
```

## Build Configuration

### Environment Variables

```bash
# Specify platform
export PLAT=linux

# Specify compiler
export CC=gcc

# Specify CFLAGS
export CFLAGS="-O2 -Wall"

# Specify Lua version
export LUA_VERSION=5.5
```

### Custom Build

Modify variables in the Makefile:

```makefile
# Custom CFLAGS
CFLAGS += -DDEBUG

# Custom library path
LIBS += -L/path/to/libs

# Custom include path
CFLAGS += -I/path/to/includes
```

## Debug Builds

### Enabling Debug Symbols

```bash
make linux DEBUG=1
```

### Enabling Address Sanitizer

```bash
make linux SANITIZE=address
```

### Enabling Memory Sanitizer

```bash
make linux SANITIZE=memory
```

## Test Builds

### Running Tests

```bash
# Start skynet node
./skynet examples/config

# Run client in another terminal
./3rd/lua/lua examples/client.lua
```

### Running Unit Tests

```bash
# Run Lua tests
./3rd/lua/lua test/test.lua

# Run C tests
make test
./test/test
```

## Installation

### System Installation

```bash
# Install to /usr/local
sudo make install PREFIX=/usr/local
```

### Custom Installation

```bash
# Install to a specified directory
make install PREFIX=/opt/skynet
```

## Build Troubleshooting

### Common Problems

1. **Lua not found**
   ```bash
   # Ensure Lua is built
   make lua
   ```

2. **jemalloc not found**
   ```bash
   # Ensure jemalloc is built
   make jemalloc
   ```

3. **Linking errors**
   ```bash
   # Check library paths
   ldd skynet
   ```

4. **Compilation errors**
   ```bash
   # Check compiler version
   gcc --version
   ```

### Cleaning and Rebuilding

```bash
# Clean all build artifacts
make cleanall

# Rebuild
make linux
```

## Next Steps

- [Project Structure](/getting-started/project-structure) - Learn about the project's directory structure
- [Core API](/getting-started/api-reference) - Learn Skynet's core API
- [Architecture Overview](/architecture/overview) - Understand Skynet's overall architecture
