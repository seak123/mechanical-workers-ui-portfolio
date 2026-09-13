# Run & verify / 运行与验证

The tests exercise independently written contracts, not the original game's executable.

测试对象是独立演示的接口约束，不是原游戏可执行程序。

## Verification status / 验证状态

Locally verified on Windows with MSVC (C++17, warnings treated as errors): **45 C++ checks**, plus the synthetic work trace. **25 Lua checks** and the authoring dry run were executed using a Lua 5.3 runtime. No third-party runtime or game dependency is included in the repository. The alternative CMake path is provided but has not been executed in this verification.

已在 Windows 验证 45 项 C++ 检查、工作流程模拟输出、25 项 Lua 检查与配置清单输出。CMake 为备用构建入口，本次未运行；没有执行真实 Unreal 集成测试。

## C++17

From an x64 Native Tools Command Prompt for Visual Studio:

```bat
build-msvc.cmd
```

Or with CMake and a C++17 compiler:

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The demo prints a **synthetic** work cycle: assign, pick up, deliver, change payload, block on energy, resume, retarget and finish.

Covered boundaries include task-vs-phase identity, update ordering, explicit completion, target changes, duplicate/stale snapshots, callback invalidation, independent status priority and same-state payload refresh.

## Lua 5.3+

Run at repository root:

```sh
lua tests/lua_tests.lua
lua tools/author_profile.lua Content/WorkerProfiles/gatherer.lua
```

Lua tests cover invalid profile references, priority conflicts, missing animations, malformed timings, lifecycle cleanup, stale callbacks, assignment eligibility and server rejection.

Lua 配置工具输出的是接入清单，不是生成 Unreal 资源。运行时需要本机安装 Lua；仓库没有打包商业项目或第三方解释器。

## Integration checks before a real game deployment

These are **not executed by this portable test suite**:

- Actor destruction and delegate teardown under Unreal object lifetime rules.
- Navigation failure, moving/despawned targets and interrupted/restarted abilities.
- World-space widget visibility, camera distance, occlusion and late asset loading.
- Localization changes and text expansion.
- Reconnect/late replication, authoritative resource accounting and actual RPC validation.
- Real-scene profiling of visible worker counts, UI update work and resource load cost.

这些属于接入真实引擎后的验证清单，不能把演示测试通过当作它们已经通过。
