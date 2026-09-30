# 来源与授权记录

来源为 xgen-core 提交 `dc5eb1167ba21de384e7a760a8586b87502b73b2`，公开前缀由 `xgc_` / `XGC_` 改为 `xgs_` / `XGS_`，路径由 `xgc/status.h` 改为 `xgen/status/status.h`。

| 本仓库 | 来源与变化 |
| --- | --- |
| `include/xgen/status/status.h` | core `include/xgc/status.h`；保留枚举数值和声明语义，补全生命周期与可选链接契约 |
| `src/status.c` | core `src/status.c`；保留全部 switch 分支与字符串，只调整命名和格式 |
| `tests/unit/test_status.cpp` | core `tests/test_core.c` 中 status 分组的行为，扩展为全部数值、字符串及 unknown 边界验证 |
| 构建与消费验证 | 参考本地 xgen-crc 试点模式，增加独立 status/strings 组件及预置 target 检查 |

原作者标记保留为 X-Gen Lab。core 的 PROVENANCE.md 记录其原工作区没有顶层 LICENSE，提取未声明新授权。本仓库继承该事实，不虚构 SPDX 或复制 roadmap 的 MIT 许可；正式公开发布前由所有者确认授权与版权。

开发 GoogleTest 采用上游 1.16.0 提交 `ff6133ab49b364a883a55ba75c39e520fea6245b`，独立准备并保留上游许可，不安装进本包。Nexus 格式与 EditorConfig 来源提交 `7a203266082ad1f655b6686713b2b7950ba31ec6`，通过 X-Gen 工程规范 1.0.0 的受控模板采用。

本次采用的规范来源为 xgen-roadmap 本地提交 `3c60419b4ab067a6d6f4ef25832d72694d9aa624` 的 `docs/standards/`。这是可定位的本地基线，不代表已经发布远端 tag。
