# 贡献指南

本仓库采用 X-Gen 工程规范 1.0.0，适用范围和权威来源见 [采用声明](docs/standards.md)。变更先确认公开状态码与字符串兼容要求，测试调用真实生产 target。

## 显式准备

开发环境需准备 CMake 3.24+、C11/C++17 编译器、Ninja 或相应原生生成器，以及 GoogleTest 1.16.0 的安装包。GoogleTest 上游提交为 `ff6133ab49b364a883a55ba75c39e520fea6245b`；使用与当前测试编译器兼容的库。配置阶段只执行 `find_package(GTest 1.16.0 EXACT CONFIG REQUIRED)`，不会下载依赖。

质量工具由显式提供的 `xgen_quality-0.1.0-py3-none-any.whl` 安装进开发虚拟环境，其 Python 依赖由 wheel 元数据固定。wheel 的准备、来源及固定修订由共享工具发布记录提供；本组件不维护第二份 requirements 或下载流程。

工具源码固定在 `tools/quality.json` 的 `quality_source.revision`。离线环境先准备当前平台的 wheelhouse，再用 `--no-index --find-links` 安装；不要求固定兄弟目录。

```sh
python -m venv out/venv
out/venv/bin/python -m pip install /absolute/path/xgen_quality-0.1.0-py3-none-any.whl
out/venv/bin/python tools/quality.py --help
```

Windows 将 `out/venv/bin/python` 替换为 `out/venv/Scripts/python.exe`。本地、pre-commit 与 CI 统一调用薄入口 `tools/quality.py`。具体命令由已安装工具的帮助列出，工具缺失时应失败并准备环境，不能跳过后声称通过。

激活虚拟环境后，用 `python -m pre_commit install` 安装钩子，`python -m pre_commit run --all-files` 执行全量快速检查。静态分析和 Doxygen 按固定版本另行准备，使用 `python tools/quality.py cppcheck --build-dir out/host`、`tidy --build-dir out/host` 与 `docs` 子命令。

## 原生构建与测试

以下命令适用于 GCC/Ninja；替换 GoogleTest 前缀为已经准备的实际安装位置：

```sh
cmake -S . -B out/host -G Ninja -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -DXGS_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_PREFIX_PATH=/absolute/gtest/install
cmake --build out/host
ctest --test-dir out/host --output-on-failure --no-tests=error
```

tests 开启时自动检查公开头在 C11/C++17 下的独立包含，并注册 unit、integration 标签。集成测试在独立目录中配置、构建、安装并运行消费者，同时验证应拒绝的错误请求。

也可使用已提供的 `host`、`release`、`coverage` presets。host/release 不固定编译器；在本机尚未选择编译器时可通过 `-DCMAKE_C_COMPILER` 和 `-DCMAKE_CXX_COMPILER` 指定。coverage 明确使用 GCC。host/coverage 默认 GoogleTest 安装前缀为 `out/deps/gtest`，允许使用 `-DCMAKE_PREFIX_PATH=/absolute/gtest/install` 显式覆盖；presets 不获取依赖。

```sh
cmake --preset host -DCMAKE_PREFIX_PATH=/absolute/gtest/install
cmake --build --preset host
ctest --preset host
```

关闭 strings 的测试构建使用另一个目录与 `-DXGS_BUILD_STRINGS=OFF`。主测试只运行常量与版本行为，但消费矩阵仍独立验证可选库的开关；生产关闭 tests 时完全不需要 C++。

## 覆盖率

使用单独目录添加 `-DXGS_ENABLE_COVERAGE=ON`，构建后执行 unit 用例。对生产 `src/status.c` 的行、函数、分支分别要求至少 80%，测试框架、C 桥和消费者不是生产分母。关闭 strings 时没有运行时实现，覆盖率为不适用，不能报虚构的 100%。具体本地证据见 [实施记录](docs/implementation-log.md)。

## 提交与发布

记录 RED/GREEN 命令、原因和回归结果，格式整理与语义变更分别说明。没有改变语义的来源提取遵循已有行为基线。提交前检查格式、公开文档、真实测试、消费矩阵和来源；不把未运行的远端 CI、sanitizer 或 MCU 测试写成通过。

本轮没有确定源代码许可证，不能直接套用其他仓库的许可。独立公开发布与下游切换需在授权声明、可获取提交和完整支持矩阵齐备后进行。

启用远端 CI 前，将仓库变量 `XGEN_QUALITY_REPOSITORY` 设置为真实可读取的工具仓库 `owner/repository`。CI 按模块配置的固定提交获取工具，缺少地址或无法获取时失败；当前未将这一准备项或远端执行标记为完成。
