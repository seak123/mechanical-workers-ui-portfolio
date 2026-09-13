# 机械兽生产线：从自动工作到清晰反馈

![机械兽在炉子附近工作，头顶文字区分不同工作阶段](media/screenshots/world-work-phases.png)

通过随行为变化的头顶状态、道具气泡与提示，让玩家读懂自动生产的当前进度与中断。

**Evan（Yaxin）Ge · C++ / Lua / UMG · ProjectZ**

[English](README.md) · [四张截图与英文图注](media/README.md)

*关注机械兽头顶文字：一只正在控火，另一只正在前往控火工作点。这些是机械兽状态，与设施标记不同。[素材来源与标签翻译](media/README.md)。*

## 我的工作

我开发、维护了生产任务、机械兽行为与配套 UI 之间的连接，包括任务切换、目标变化、搬运物件表现、头顶反馈的接入迭代，以及机械兽配置工作流。

本案例关注 gameplay 状态如何转成可读反馈，以及不同工种如何使用共同的配置关系。

## 三个问题与决策

### 1. 不同工种如何共用可预测的反馈系统？

机械兽遵循共同的基类行为树工作流，已有工种通过 Task 参数、工作 GA 和内容数据表达差异。我沿用这一约定驱动头顶反馈：能力驱动的提示由 Task 激活 GA，再通过状态 Tag 进入 UI 状态映射。

UI 选择配置的表现形式，并填入当前工作数据；AI 状态与中断原因是互补输入。Task 提供行为含义，不必直接操纵控件。[决策故事](docs/DECISIONS.md) · [配置工作流](docs/AUTHORING.md)。

### 2. 表现阶段变化，但任务没有变化时，应该如何处理？

前往工作点与实际工作可以是同一任务的两个阶段。我维护生产与行为的接入，让机械兽当前数据先转成任务描述，再更新表现。

当前／上次任务与明确的目标变化重启路径，把移动、换目标与阶段切换连接到实际分配的工作。[任务切换](docs/CASE_STUDIES.md#one-assignment-many-phases) · [接口路线](docs/CODE_TOUR.md#1-production-state-to-current-job)。

### 3. 状态不变时，如何保持具体显示正确？

“搬运中”可以持续不变，但搬运物品已经变化。我维护任务应用与搬运物件刷新之间的顺序，让表现读取新任务。

头顶文字与气泡也分别处理状态和内容：状态、优先级决定提示形式，物品数据决定显示什么。相同形式可以承载不同工种和物品。[状态与内容一致性](docs/CASE_STUDIES.md#state-and-payload-consistency)。

## 实际结果

- 已有工种共用反馈语义，同时保留文字、图像与动画配置差异。
- 任务切换与目标变化有明确的行为接入和诊断入口。
- 搬运物件在任务生效后读取新数据，头顶反馈在大状态不变时仍可更新物品内容。

## 深入阅读

- **案例与架构：**[具体案例](docs/CASE_STUDIES.md) · [架构](docs/ARCHITECTURE.md) · [决策](docs/DECISIONS.md)。
- **实现：**[真实接口与调用关系导读](docs/CODE_TOUR.md)。
- **内容工作流：**[Actor、工作 GA 与头顶反馈配置](docs/AUTHORING.md)。
- **诊断：**[对比任务、移动与可见行为](docs/CASE_STUDIES.md#observe-before-guessing)。
- **可执行参考：**[独立模型，非原项目代码或原编辑工具](docs/REFERENCE_APPENDIX.md) · [参考模型测试](docs/TESTING.md)。
- **材料范围：**[历史工作、接口说明、模型与素材](docs/EVIDENCE.md)。

关键词：shared behaviour contract · status-tag mapping · presentation versus payload · data-driven feedback。

**其他案例：**[建造 UI](https://github.com/seak123/building-ui-portfolio) · [组队 UI](https://github.com/seak123/multiplayer-ui-portfolio)。
