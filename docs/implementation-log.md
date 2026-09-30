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

- 共享质量 runner、CI 配置及最终门禁由主任务接入，尚未在此记录为通过。
- 未执行 Linux sanitizer、ARM 链接或真实硬件验证；本组件不单独声称满足整机资源预算。
- 未创建远端、推送或发布 tag，未修改 memory/containers/link 等消费者。
- 来源许可证仍需所有者确认，不能声明已经具备正式公开发布条件。

????? pre-commit ?????????????? `python -m pre_commit run --all-files`???/??? Nexus C/C++ ???????`git diff --cached --check` ????????????????????????? CI ????????????? `XGEN_QUALITY_REPOSITORY` ?????????
