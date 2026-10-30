# 架构决策记录（ADR）

每个影响架构、不容易推翻的决定都记一篇，写清背景、考虑过的方案、最终决定和后果。格式参照 Michael Nygard 的 ADR 模板。

- 文件命名：`NNNN-英文短标题.md`，编号递增，不复用。
- 已采纳的 ADR 不改内容。决定变了就新写一篇，再把旧的一篇的状态改成"已被 ADR NNNN 取代"。

| 编号 | 标题 | 状态 |
|---|---|---|
| [0001](0001-platform-server-in-cpp-with-drogon.md) | 平台服用 C++20 和 drogon | 已采纳 |
| [0002](0002-battle-server-as-unity-dedicated-server.md) | 战斗服用 Unity 专用服构建 | 已采纳 |
| [0003](0003-session-tokens-and-join-tickets.md) | 会话 token 和加入票据分开 | 已采纳 |
