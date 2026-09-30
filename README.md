# xgen-status

独立的 C11 通用状态码组件。本地包版本为 0.1.0，ABI 为 1；尚未创建远端或正式发布。来源和授权状态见 [PROVENANCE.md](PROVENANCE.md)。

| Target | 内容 | 依赖 |
| --- | --- | --- |
| `xgs::status` | 状态类型、固定数值及公开头，INTERFACE target | 无 |
| `xgs::strings` | 可选的 `xgs_status_string` 静态库 | `xgs::status` |

状态码保持来源的 `0` 至 `-7` 数值和英文字符串。`xgs_status_string` 对未知值返回 `"Unknown status"`，返回内容不可修改或释放，有效期为整个程序。组件没有堆分配、平台、时间源、线程或可变全局状态，不需要初始化。枚举的内存表示由目标 C ABI 决定，不能直接作为线格式。

公开入口为 `<xgen/status/status.h>` 和生成的 `<xgen/status/version.h>`。包含状态头不会引入字符串实现；只有调用字符串函数的消费者才需要显式链接 `xgs::strings`。

## 生产构建与安装

需要 CMake 3.24 及以上、C11 编译器和相应构建工具。普通配置不联网、不要求 C++ 或 GoogleTest。以下为 GCC/Ninja 示例：

```sh
cmake -S . -B out/release -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_BUILD_TYPE=Release
cmake --build out/release
cmake --install out/release --prefix /absolute/install/prefix
```

| 选项 | 默认值 | 说明 |
| --- | --- | --- |
| `XGS_BUILD_STRINGS` | ON | 构建并安装可选字符串实现 |
| `XGS_BUILD_TESTS` | OFF | 启用主机 GoogleTest 与消费测试 |
| `XGS_ENABLE_COVERAGE` | OFF | 对真实 strings 对象启用覆盖率，需要主机 tests 和 strings |

只需要类型时设置 `-DXGS_BUILD_STRINGS=OFF`。该配置没有生产 `.c` 对象，仍可以构建、安装并被纯 C 工程消费。

## 安装包消费

在已有 CMake `application` target 的工程中，显式提供安装前缀或 `xgen_status_DIR`，然后选择组件：

```cmake
find_package(xgen_status 0.1.0 EXACT CONFIG REQUIRED COMPONENTS status)
target_link_libraries(application PRIVATE xgs::status)
```

不指定 `COMPONENTS` 时也只导入 `status`，即使安装包包含 strings，也不会导入其 target。需要诊断时改为：

```cmake
find_package(xgen_status 0.1.0 EXACT CONFIG REQUIRED COMPONENTS strings)
target_link_libraries(application PRIVATE xgs::strings)
```

strings 自动带入 status 的使用要求。必需组件缺失、未知组件、版本不兼容或预置 target 的版本/ABI/类型不符合要求时配置失败。未知的可选组件报告未找到，不使其余有效组件失败。

## 源码消费与父工程 target

使用显式源码路径，不依赖固定兄弟目录：

```cmake
set(XGS_BUILD_STRINGS OFF CACHE BOOL "Only status types are required")
add_subdirectory("${XGEN_STATUS_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/xgen-status")
target_link_libraries(application PRIVATE xgs::status)
```

默认 strings 为 ON，源码消费仅需类型时应在首次加入前显式关闭。预先存在的 `xgs::` target 必须带有 `XGS_VERSION=0.1.0`、`XGS_ABI_VERSION=1`，并具有正确类型。源码重复接入只复用完整、兼容的已选 target 集合，不重新安装或测试上一个提供者的 target；不完整集合被拒绝。包作者提供属性，消费者不能靠伪造属性修复不兼容二进制。

## 开发与状态

- [贡献与验证入口](CONTRIBUTING.md)
- [工程规范采用](docs/standards.md)
- [提取与验证记录](docs/implementation-log.md)
- [变更记录](CHANGELOG.md)

生产代码来自 core 的固定提交，公开前缀和头路径已改变，不提供旧 `xgc_` 转发层。协议、memory、containers 等消费者的迁移属于后续集成任务。本库验证不代替完整 MCU 镜像和实际硬件验收。
