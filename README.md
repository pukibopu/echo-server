# echo-server

Platform backend for ECHO (回波): accounts, room codes, matchmaking, dedicated-server allocation, match results and leaderboards. C++20 · drogon · MySQL · Redis.

《回波》的平台服，管对局之外的事：账号、房间码和匹配、拉起战斗服、结算、战绩和排行榜。对局里的判定由 Unity 专用战斗服负责，不在这个仓库里。


## 运行

需要 CMake 3.31 以上、Ninja、vcpkg（设好 `VCPKG_ROOT`）和 Docker。

```bash
cp .env.example .env          # 再把里面的密码改掉
docker compose up -d --wait   # 启动 MySQL 和 Redis
set -a && . ./.env && set +a  # 把 .env 里的变量导出到当前终端
cmake --preset debug && cmake --build --preset debug
ctest --preset debug -L unit
ctest --preset debug -L integration   # 要连上面启动的 MySQL 和 Redis
./build/debug/apps/platform/echo-platform config/platform.example.json
```

平台服的设置在 `config/platform.example.json` 里，要改就复制一份 `config/platform.json` 再改（这个文件不会被提交）。MySQL 密码只从环境变量 `MYSQL_PASSWORD` 读。启动时连不上 MySQL 或 Redis，会打印原因并退出。

## 版权

Copyright © 2026 Shuyang Yan. All rights reserved.

本仓库仅供展示和交流，未授予任何使用、复制、修改或分发的许可。
