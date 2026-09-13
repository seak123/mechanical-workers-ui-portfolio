# 机械兽生产线 — 玩法、世界空间 UI 与配置工作流

**让自动生产过程可理解、可配置，并保持表现一致。**

Evan Ge 的玩法与 UI 工程案例。玩家可以安排机械兽采集、搬运、制造和维护家园。工作会自动进行，但界面必须说明：它正在做什么、为什么停下来，以及玩家可以怎样处理。

[English](README.md) · [实际案例](docs/CASE_STUDIES.md) · [配置工作流](docs/AUTHORING.md) · [架构与代码导读](docs/ARCHITECTURE.md) · [画面素材](media/README.md) · [运行与测试](docs/TESTING.md)

![配置与运行时的概念关系图，并非游戏截图](docs/assets/system-map.svg)

## 我的工作背景

我开发、维护了生产任务与机械兽行为表现之间的接入层，包括接收工作、移动与工作表演切换、目标变化和中断处理，以及搬运物件的显示一致性。我也参与了配套 UI 的接入与迭代，以及机械兽配置工作流的建设。完整生产系统和共享 UI 基础设施由团队协作开发。

原项目采用 **C++、Lua、Gameplay Ability、Actor 蓝图与世界空间 UMG 控件**。

## 三条工程主线

### 1. 一次工作，有多个表现阶段

拾取、移动、送达属于同一次搬运任务，不能把每个阶段都当作“接到新工作”。我参与梳理了工作数据如何驱动当前任务和表现提示，也处理了工作中目标变化后的移动重启。

价值在于：切换更可预测，减少重复或矛盾提示，新工作类型有明确的接入位置。[详细案例](docs/CASE_STUDIES.md#one-assignment-many-phases)

### 2. 机械兽和 UI 必须表达同一件事

缺少能量的状态可能被映射成闲置表现；搬运物件也可能在新任务数据生效前就被刷新。这些问题跨越玩法状态、角色表现和界面反馈，关键在于状态含义与更新顺序，而不只是修改控件。

价值在于：玩家能区分等待、工作和需要干预的情况。[详细案例](docs/CASE_STUDIES.md#state-and-payload-consistency)

### 3. 配置内容，也能检查配置背后的状态

配置流程连接机械兽 Actor 蓝图、工作 GA，以及各状态对应的文字、图标和动画。头顶 UI 的状态配置与定向调试入口，可以让开发者直接观察状态组合，而不必每次重建完整生产环境。

价值在于：策划、美术和工程师可以围绕明确的接口关系迭代。[配置工作流](docs/AUTHORING.md)

## 代码阅读入口

- [WorkModel](Source/Workers/WorkModel.cpp)：任务身份、阶段切换、先更新数据再通知表现。
- [HeadPresenter](Source/Workers/HeadPresenter.cpp)：文字与气泡的独立优先级、状态不变时的内容刷新。
- [WorkerPanel.lua](Content/Lua/WorkerPanel.lua)：管理面板生命周期、条件反馈、权威请求。
- [AuthoringProfile.lua](Content/Lua/AuthoringProfile.lua)：独立配置校验与接入清单生成示例。
- [C++ 测试](tests/worker_tests.cpp)与 [Lua 测试](tests/lua_tests.lua)：可执行的边界条件。

这里的代码是为案例**重新编写的可移植参考实现**，不是商业项目源码或可运行游戏。它演示部分接口约束，不包含生产调度器、Unreal 资源和完整引擎接入。文档区分原项目经验与演示中新加入的防护措施。

## 展示素材

暂未加入实际游戏截图。[素材计划](media/README.md)列出了需要补充的镜头和英文图注。当前关系图与终端输出均为说明性材料，不作为游戏交互过程的实录。

[建造与物件交互 UI](https://github.com/seak123/building-ui-portfolio) · [组队、匹配与支援 UI](https://github.com/seak123/multiplayer-ui-portfolio)
