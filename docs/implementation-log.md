# 独立 status 提取记录

日期：2026-10-01。分支：`feat/independent-status`。源基线为 core `dc5eb1167ba21de384e7a760a8586b87502b73b2`。本轮仅操作新的 status 仓库，原 core/link 未修改；最终提交、共享质量接入及路线图更新由主任务统一处理。

## 行为基线

主任务在提取前重跑来源 status/bytes 两组测试，2/2 通过。本次迁移保留 status 的全部数值、文本和 unknown 分支，公开路径/前缀独立化；未重写 switch 算法。GoogleTest 测试覆盖八个已知值、文本存活期和七个 unknown 整数。

C++ 不对范围外枚举进行强制转换。unknown 测试由 `tests/support/status_from_int.c` 接收 int 并在 C 中转换，随后调用真实 `xgs_status_string`，保持原 C 接口语义。

## 构建契约 TDD

初始提取保留来源将类型和字符串放入 STATIC target 的结构。新增 `tests/integration/status_only_red` 断言 status-only 必须是 INTERFACE 且不能生成 strings。运行：

```sh
cmake -S tests/integration/status_only_red -B out/red-status-only-gcc -G Ninja -DCMAKE_C_COMPILER=gcc -DXGS_SOURCE_DIR=<absolute-status-checkout>
```

实际退出 1，明确诊断 `status-only contract requires INTERFACE_LIBRARY; got STATIC_LIBRARY`；日志为 `out/evidence/status-only-red-gcc.log`。这是目标契约未实现的失败，不是缺工具。此前一次未指定编译器的配置选中 clang，因 MSVC 运行库未配置而失败，单独保存且不计 RED。

将类型拆为 INTERFACE、字符串拆为可选 STATIC 后，同一契约在 `out/green-status-only` 配置退出 0，日志为 `out/evidence/status-only-green.log`。之后完整消费矩阵覆盖此契约及依赖拒绝行为。按主任务分工未创建子任务 checkpoint 提交，证据保留在记录与工作区中。

## 本地验证

| 配置 | 实际结果 |
| --- | --- |
| GCC 13.2.0 / Debug | 45/45 CTest 通过，19 个 unit、26 个 integration |
| GCC 生产 coverage / strings ON | 19/19 unit 通过；行 20/20、函数 1/1、分支 9/9，均 100% |
| MSVC 19.40 / x64 Debug | 初始 39/39 与新增 6/6 消费回归均通过，合计覆盖最终 45 项 |
| GCC / strings OFF 主机单元 | 9/9 unit 通过，C/C++ 头检查均完成；无字符串对象 |
| strings OFF 纯 C 安装消费者 | 已在消费矩阵中实际配置、构建、安装与运行 |

覆盖率使用 gcovr 8.3 对真实生产对象读取，分母仅 `src/status.c`；报告为 `out/coverage/summary.json`。Header-only 目标没有生产函数，不能把其覆盖率写为 100%。

每个消费测试在 `out/host/integration/<case>/<run-id>` 创建独立目录并保存生产者、消费者配置/构建/安装/运行日志。负例核对失败原因，避免把无关编译错误当作预期拒绝。默认包导入、显式 status、strings 及关闭 strings 的安装包分别验证。

## 待完成与适用限制

- 共享质量 runner 已完成本地验收，见下节；CI 配置已提供，远端工作流尚未执行。
- 未执行 Linux sanitizer、ARM 链接或真实硬件验证；本组件不单独声称满足整机资源预算。
- 未创建远端、推送或发布 tag，未修改 memory/containers/link 等消费者。
- 来源许可证仍需所有者确认，不能声明已经具备正式公开发布条件。

## 共享质量工具验收（2026-10-01）

组件现在调用显式安装的 xgen-quality 0.1.0，源码固定为 `9c957d406d935d27959babe7f01172151f9d26c1`。检查逻辑、工具策略和回归由该独立仓库维护；本仓只保留模块配置与薄入口。最终 wheel 的 SHA256 为 `7a1946e9256d12e49365f064fd1fc44533d58e62addccc6b91268e2d657443f7`；声明的源码提交和实际 wheel 来源分别记录，不能相互代替。

Windows 本地实际完成 text、clang-format 19.1.5、Doxygen 1.16.0、cppcheck 2.21.0、clang-tidy 19.1.0、CTest 与 gcovr 8.3 检查，全部通过。原始结果在 `out/reports/quality-*.json`；工具自身的 66 项 Python 回归在 xgen-quality 执行，组件不再复制这套测试。实现阶段的报告保留当时源码和安装身份；最终 wheel 更新了模板与元数据，运行 Python 和 policy 内容已逐字节核对为受测版本。

已安装本地 pre-commit 钩子；暂存完整自有文件后运行 `python -m pre_commit run --all-files`，文本/配置与 Nexus C/C++ 格式两项通过，`git diff --cached --check` 通过。三个消费者固定到同一工具提交。远端 CI 仍需真实可读取的工具仓库及 `XGEN_QUALITY_REPOSITORY` 变量，当前未执行。

共享 runner 运行最终 45/45 CTest 通过；自有生产对象的行、函数与分支覆盖率均为 100%。独立虚拟环境的固定依赖安装及 pip check 通过。

## 干净克隆与共同安装验证

在包含空格的新目录中，分别从 CRC `358659cfdf85718d2472dd9aa43bc5cf704d91c9`、bytes `9ce17afa3a572e9aa9db9a2f4e8f4b3802b6326f`、status `d60c8e02f0720b03f30d7ca725bd8834ccf7add3` 创建 `--no-hardlinks` 干净本地克隆，不复制开发缓存或 out。使用 GCC 13.2，保持 tests 默认 OFF，将 CXX 指向不存在路径后，三个纯 C Release 均完成配置、构建和安装。

共同安装前缀新增 CRC 10、bytes 8、status 9 个文件，共 27 个；安装清单没有交叉路径，每次安装前已有文件的 SHA256 均不变。三个生产工程与消费者的缓存均无 CXX/GoogleTest 条目。

真实消费者最小路径只导入 `xgcrc::crc8`、`xgb::bytes`、`xgs::status`，符号检查确认可执行文件不含 CRC16、status strings 和 GoogleTest；另一路径显式选择 strings。两个消费者的 CTest **2/2 通过**。此结果是基础库组合验证，不代表产品、ARM 或 Boot 验收。

可复现脚本和含 39 条命令、36 个检查、源提交及安装摘要的完整报告保存在本工作区 CRC 的 `out/verify_shared_install.py` 与 `out/reports/shared-install.json`。这些构建和检查产物由 out 排除在源码之外；后续提交仅补充和修正文档，不改已验证的生产实现。
