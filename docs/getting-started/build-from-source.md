# 从源码构建

在实际项目中，我们通常需要从源码构建 Skynet，这样可以方便地进行调试和定制。这里记录了构建过程中的经验。

## 系统要求

### Linux

- GCC 或 Clang
- GNU Make
- autoconf (用于 jemalloc)

### macOS

- Xcode Command Line Tools
- GNU Make (gmake)

### FreeBSD

- GCC 或 Clang
- GNU Make (gmake)

## 获取源码

```bash
git clone https://github.com/cloudwu/skynet.git
cd skynet
```

## 构建 Skynet

### Linux

```bash
make linux
```

或者：

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

## 构建选项

### 指定平台

```bash
make PLATFORM
# PLATFORM 可以是: linux, macosx, freebsd, openbsd
```

### 清理构建

```bash
make cleanall
```

### 仅构建 Lua

```bash
make lua
```

### 仅构建 skynet

```bash
make skynet
```

## 构建系统结构

### Makefile 结构

```
Makefile              # 主构建文件
├── platform.mk       # 平台相关配置
├── mingw.mk          # MinGW (Windows) 配置
└── 3rd/
    ├── lua/          # Lua 构建
    ├── jemalloc/     # jemalloc 内存分配器
    └── lpeg/         # LPEG 模块
```

### 主 Makefile

```makefile
# 主要构建目标
all : $(ALL)

# Skynet 核心
SKYNET_SRC = skynet-src

# 源文件
SKYNET_DEFINES =
CSERVICE = cservice

# 构建 skynet
skynet : $(LUA_LIB) $(JEMALLOC_STATICLIB) $(SKYNET_DEFINES)
	$(CC) $(CFLAGS) -o $@ ...
```

### 平台配置

```makefile
# platform.mk

# Linux 配置
ifeq ($(PLAT),linux)
  CFLAGS += -DUSE_$(EXPORT_FLAG)
  SHARED := -shared
  LIBS += -lpthread -lrt
endif

# macOS 配置
ifeq ($(PLAT),macosx)
  SHARED := -bundle -undefined dynamic_lookup
endif
```

## 核心组件构建

### 1. Lua

Skynet 使用修改版的 Lua 5.5.0：

```bash
# 构建 Lua
make lua

# 输出文件
3rd/lua/lua        # Lua 解释器
3rd/lua/luac       # Lua 编译器
3rd/lua/liblua.a   # Lua 静态库
```

### 2. jemalloc

高性能内存分配器：

```bash
# 构建 jemalloc
make jemalloc

# 输出文件
3rd/jemalloc/lib/libjemalloc.a
```

### 3. LPEG

Lua 模式匹配库：

```bash
# 构建 LPEG
make lpeg

# 输出文件
luaclib/lpeg.so
```

## 服务模块构建

### C 服务

C 服务模块编译为 `.so` 文件：

```bash
# 构建所有 C 服务
make cservice

# 输出目录
cservice/
├── logger.so      # 日志服务
├── gate.so        # 网关服务
├── harbor.so      # 集群服务
└── ...
```

### Lua C 模块

Lua 的 C 扩展模块：

```bash
# 构建所有 Lua C 模块
make luaclib

# 输出目录
luaclib/
├── bson.so        # BSON 支持
├── md5.so         # MD5 哈希
├── lpeg.so        # LPEG 模式匹配
└── ...
```

## 构建配置

### 环境变量

```bash
# 指定平台
export PLAT=linux

# 指定编译器
export CC=gcc

# 指定 CFLAGS
export CFLAGS="-O2 -Wall"

# 指定 Lua 版本
export LUA_VERSION=5.5
```

### 自定义构建

修改 Makefile 中的变量：

```makefile
# 自定义 CFLAGS
CFLAGS += -DDEBUG

# 自定义库路径
LIBS += -L/path/to/libs

# 自定义包含路径
CFLAGS += -I/path/to/includes
```

## 调试构建

### 启用调试符号

```bash
make linux DEBUG=1
```

### 启用地址消毒器

```bash
make linux SANITIZE=address
```

### 启用内存消毒器

```bash
make linux SANITIZE=memory
```

## 测试构建

### 运行测试

```bash
# 启动 skynet 节点
./skynet examples/config

# 在另一个终端运行客户端
./3rd/lua/lua examples/client.lua
```

### 运行单元测试

```bash
# 运行 Lua 测试
./3rd/lua/lua test/test.lua

# 运行 C 测试
make test
./test/test
```

## 安装

### 系统安装

```bash
# 安装到 /usr/local
sudo make install PREFIX=/usr/local
```

### 自定义安装

```bash
# 安装到指定目录
make install PREFIX=/opt/skynet
```

## 构建问题排查

### 常见问题

1. **找不到 Lua**
   ```bash
   # 确保 Lua 已构建
   make lua
   ```

2. **找不到 jemalloc**
   ```bash
   # 确保 jemalloc 已构建
   make jemalloc
   ```

3. **链接错误**
   ```bash
   # 检查库路径
   ldd skynet
   ```

4. **编译错误**
   ```bash
   # 检查编译器版本
   gcc --version
   ```

### 清理并重新构建

```bash
# 清理所有构建产物
make cleanall

# 重新构建
make linux
```

## 下一步

- [项目结构](/getting-started/project-structure) - 了解项目的目录结构
- [核心 API](/getting-started/api-reference) - 学习 Skynet 的核心 API
- [架构概览](/architecture/overview) - 理解 Skynet 的整体架构
