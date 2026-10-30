# echo-server

> Platform backend for **ECHO (回波)**, a first-person LiDAR infiltrator-vs-hunter game. It handles accounts, room codes, matchmaking, dedicated-server allocation, match results and leaderboards. C++20 · drogon · MySQL · Redis.

《回波》的平台服。游戏是第一人称、靠 LiDAR 扫描的 1v1 攻守对抗，玩家分潜入者和猎手。对局里的判定由 Unity 专用战斗服负责。本仓库管对局之外的部分：账号、开局（房间码和匹配）、拉起战斗服、结算、战绩和排行榜。

> **状态**：开发中，当前在 S0（环境和仓库）。各阶段见[路线](#路线)。

## 架构

```
Unity 客户端 ──HTTPS/JSON──► echo-platform（本仓库，C++20 / drogon）──► MySQL 8（账号、对局、战绩，以它为准）
     │        ──WebSocket──►   │  匹配推送 / 房间码                   └─► Redis 7（会话、排行榜、房间码）
     │                         │  内部接口：战斗服就绪 / 心跳 / 结果
     │                         ▼  分配器：按对局拉起战斗服进程
     └──UDP（Netcode for GameObjects）──► Unity 专用战斗服（每局一个进程，跑权威规则层）
```

- **两层权威**：对局里的判定以战斗服为准；对局之外的数据以平台服和 MySQL 为准。
- **两种凭证**：会话 token 存在 Redis，可以随时吊销；进战斗服用的加入票据是无状态的 HMAC 签名，战斗服不用连 Redis。
- 详细设计见 [docs/architecture.md](docs/architecture.md)，关键决定的取舍见 [docs/adr/](docs/adr/)。

## 路线

| 阶段 | 内容 | 版本 |
|---|---|---|
| S0 | 开发环境、仓库规范、CI、健康检查接口 | — |
| S1 | 账号：注册、登录、游客登录、会话、鉴权 | v0.1.0 |
| S2 | 匹配核心库：按 rating 匹配、窗口随等待时间扩大、Elo 结算 | v0.2.0 |
| S3 | 匹配和房间码服务、拉起战斗服、结算 | v0.3.0 |
| S4 | 战绩、排行榜、监控指标、压测 | v0.4.0 |

## 技术栈

| 用途 | 选型 |
|---|---|
| 语言 | C++20（主体按 C++17 风格写，只在异步 IO 处用协程） |
| Web 框架 | [drogon](https://github.com/drogonframework/drogon)：HTTP、WebSocket，自带 MySQL 和 Redis 客户端 |
| 存储 | MySQL 8（以它为准）、Redis 7（会话、排行榜、房间码） |
| 密码与签名 | libsodium（Argon2id、HMAC-SHA256） |
| 构建 | CMake + Ninja + vcpkg（清单模式，锁定 baseline） |
| 测试 | GoogleTest + CTest |
| 运维 | Docker Compose、Prometheus、Grafana、GitHub Actions |

## 目录

| 路径 | 内容 |
|---|---|
| `apps/platform/` | 平台服进程 |
| `apps/fake-ds/` | 假战斗服，Unity 专用服做出来之前用来跑端到端测试和压测 |
| `apps/loadtest/` | 压测机器人 |
| `libs/matchmaking/` | 匹配和 Elo 的纯 C++ 库，不做 IO |
| `libs/common/` | 公共组件：票据、错误类型、配置 |
| `api/openapi.yaml` | HTTP 接口契约 |
| `db/migrations/` | 数据库迁移脚本 |
| `deploy/` | Dockerfile、Prometheus 和 Grafana 配置 |
| `docs/` | 设计文档和架构决策记录（ADR） |

目录会随阶段补齐，上表是最终结构。

## 快速开始

开发用 macOS（Apple 芯片），加 Docker Desktop；CI 和上线环境是 Linux x64。构建命令在 S0 骨架完成后补上。

## 版权

Copyright © 2026 Shuyang Yan. All rights reserved.

本仓库仅供展示和交流，未授予任何使用、复制、修改或分发的许可。
