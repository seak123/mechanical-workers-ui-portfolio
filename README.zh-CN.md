# 机械兽生产线：从自动工作到清晰反馈

把生产任务连接到玩家可读的 UI：分配机械兽、观察当前工作，并通过气泡辨认正在处理的物品。

**Evan（Yaxin）Ge · C++ / Lua / UMG · ProjectZ**

[English](README.md) · [功能截图](#功能截图) · [我的工作](#我的工作) · [深入阅读](#深入阅读)

## 功能截图

三张图展示分配上下文、世界中的工作状态与物品反馈。[全部四张图、英文标签和素材来源](media/README.md)。

### 生产编队：玩家分配工作的上下文

![左侧生产编队、中间后备机械兽、底部家园动力](media/screenshots/production-roster.png)

**功能：**同时呈现生产编队、后备机械兽与家园动力；画面中已有两只机械兽进入容量为十的编队，为选择劳动力提供上下文。

**与案例的连接：**从玩家分配进入后续生产与行为接入，由机械兽当前数据形成工作任务，再观察对应的世界反馈。

[功能上下文与管理界面边界](docs/ARCHITECTURE.md#management-ui-boundary) · [生产状态到当前任务](docs/CODE_TOUR.md#1-production-state-to-current-job)

### 头顶状态：区分前往工作与实际工作

![炉子附近机械兽的控火中与控火移动中头顶文字](media/screenshots/world-work-phases.png)

**功能：**不打开管理界面，也能从机械兽头顶分辨“控火中”和“控火移动中”。机械兽状态与设施标记是不同 UI。

**相关工作：**任务与阶段驱动行为，能力提示由 Task → GA → 状态 Tag → 头顶 UI 映射连接，AI 状态与中断原因提供互补输入，配置决定文字和气泡表现。

[同一任务的多个阶段](docs/CASE_STUDIES.md#one-assignment-many-phases) · [共同约定的决策](docs/DECISIONS.md) · [头顶状态与生命周期](docs/CODE_TOUR.md#3-head-ui-state-and-lifecycle)

### 物品气泡：告诉玩家搬运的是什么

![搬运中的机械兽及头顶木材图标](media/screenshots/transport-item-bubble.png)

**功能：**“搬运中”文字说明动作，木材图标说明对象，二者共同表达当前工作。

**相关工作：**先应用新任务再刷新搬运物件，并区分宽泛状态与具体数据；即使始终在搬运，换了物品也要更新反馈。

[状态与内容一致性](docs/CASE_STUDIES.md#state-and-payload-consistency) · [文字与气泡实现](docs/CODE_TOUR.md#4-text-and-bubble-specialisation) · [配置工作流](docs/AUTHORING.md)

**其他截图：[设施生产暂停](media/README.md#3-facility-production-state)。**工作台显示生产状态、剩余数量、机械兽与材料上下文，这是关联设施视图，与头顶反馈分开。

**配置入口：[Actor BP、工作 GA 与头顶文字／气泡配置](docs/AUTHORING.md)。**通过接口和流程说明呈现配置关系，该章节没有原编辑器截图。

## 我的工作

我开发、维护了生产任务、机械兽行为与配套 UI 之间的连接，工作覆盖运行时接入、表现一致性、头顶反馈迭代与配置工作流。

- **任务与行为：**将机械兽状态转换成当前工作，协调新旧任务及移动、表演、完成与中断阶段。
- **目标变化：**维护重新定向路径，让新目标进入移动和行为，避免继续跟随旧工作位置。
- **头顶反馈：**连接行为／能力信号与文字、气泡，使用共用状态映射并保留工种内容差异。
- **状态与物品一致性：**维护任务应用和搬运表现的顺序，把活动状态与具体物品当成两个输入。
- **内容配置：**参与连接机械兽 Actor BP、工作 GA 与头显的流程，包括文字、图像、动画和优先级配置。
- **诊断与迭代：**添加、维护任务、移动目标、AI 状态与表现的对照信息，定位数据在哪一层开始不一致。

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
