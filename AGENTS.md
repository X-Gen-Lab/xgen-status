# 仓库工作约定

- 始终使用简体中文与用户沟通。
- 开始前检查 Git 状态，读取 [贡献指南](CONTRIBUTING.md)、[规范采用声明](docs/standards.md) 与相关公开接口；保护已有修改。
- 新行为和构建契约执行真实 RED/GREEN，纯提取先验证来源行为，不人为破坏算法制造 RED。
- 生产代码使用 C11；主机测试使用 C++17 和显式准备的 GoogleTest 1.16.0。测试依赖不进入生产包。
- 自有代码使用根 `.clang-format` 的 X-Gen 格式及英文反斜杠 Doxygen 注释。版本模板不是有效 C，不直接对 `.h.in` 运行 formatter；实际生成头通过包含和安装消费验证。
- 质量入口为 `python tools/quality.py`，共享实现由显式安装的 xgen-quality 提供。不要复制 runner、工具 requirements 或规则实现。
- 不修改其他组件仓库，不添加 core 兼容包装，不擅自声明来源代码许可证或创建远端。
- 分别记录本地执行、远端 CI、交叉链接与硬件验证；没有执行的检查不填写通过。
