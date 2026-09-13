# Architecture & code tour / 架构与代码导读

[Overview](../README.md) · [Stories](CASE_STUDIES.md)

![Conceptual relationship between authoring, work execution and player feedback](assets/system-map.svg)

## Production architecture / 实际系统分层

The original system has several distinct responsibilities:

1. **Work scheduling:** choose and maintain work, production objects and workers; account for capacity, energy and interruptions.
2. **Work-state data:** communicate the creature's current state, destination and associated payload.
3. **Execution/presentation:** map that data into a job, movement and ability-driven behaviour.
4. **World-space UI:** show status text, item/emoji bubbles and resource-related feedback.
5. **Management UI:** let the player inspect a worker, understand eligibility and request assignment/return actions.
6. **Authoring/debugging:** configure content and inspect the state-to-behaviour-to-feedback chain.

Scheduling is not owned by a widget, and completing a visual animation is not equivalent to authoritative production completion.

原系统区分工作调度、状态数据、行为表现、头顶 UI、管理面板和配置调试。控件不拥有生产调度；动画结束也不等于权威生产结果已经完成。

## Reference implementation / 演示实现

### Work variety shapes the contract / 工作类型决定接口需求

The production feature is broader than a single gather loop. Its work families include resource collection/mining, planting/watering, crafting/fire control, repair, charging and transport. These stress different parts of the integration:

- **Transport:** pickup and delivery phases, changing item identity, carried-object presentation.
- **Targeted work:** a destination can change while the worker is moving or performing.
- **Charging:** identifying the charging object and its slot matters, not just showing a generic work animation.
- **Interrupted or blocked work:** energy, motivation and unavailable targets must survive the mapping into visible feedback.
- **Ambient performance:** emote/idle behaviour has its own timing and must coexist with real work state.

The sample deliberately implements fewer work families so the contracts remain readable. It does not reproduce the scheduler or every production ability.

实际生产线包含采集/采矿、种植/浇水、制造/控火、修理、充电和搬运等工作。搬运需要处理阶段与物品身份，定点工作需要应对目标变化，充电还涉及具体对象与槽位。中断原因不能在表现映射中丢失，闲置/表情行为也要与真实工作区分。演示代码只保留代表性的工作类型，重点展示这些类型如何影响接口，而不是复刻整套调度器。

### Sample call chain / 演示调用链

```text
Content/WorkerProfiles/gatherer.lua
    -> AuthoringProfile.validate / plan
    -> symbolic Actor + GA + head-status integration checklist

Authoritative adapter [not implemented here]
    -> Snapshot
    -> WorkModel.Apply
         -> accepted current state
         -> movement / ability / prop effects [engine adapter omitted]
    -> HeadPresenter.Refresh(accepted current state)
         -> text / icon / animation deltas [UMG adapter omitted]

WorkerPanel.lua
    -> service.snapshot / subscribe
    -> view.render / feedback
    -> service.requestAssignment
         -> acknowledgement or server rejection
```

No live Lua/C++ binding, replication transport, navigation system or Blueprint implementation is included. The Lua panel tests use a fake service. The C++ tests exercise the native contract independently.

没有提供完整 Lua/C++ 绑定、网络复制、导航和蓝图实现。Lua 测试使用模拟服务，C++ 测试独立检查原生层约束，因此不把它描述成可直接运行的原游戏模块。

## Key contracts / 关键约束

### Identity is not phase / 任务身份不同于阶段

`assignment` identifies a semantic work cycle. Travel and performance are phases within it. Changing the payload does not necessarily change the assignment. A changed target can require a movement restart without a new-job cue.

In this reference model, `revision` rejects duplicate/out-of-order snapshots per bound worker. It is a simplified integration policy, not an implementation of the game's replication protocol.

### Apply data before refreshing presentation / 先更新数据，再刷新表现

`WorkModel::Apply` commits the accepted snapshot before returning visual effects. Consumers can read the same updated job. The actual engine adapter must process effects on the appropriate thread and keep binding/lifetime checks around asynchronous work.

`PropBinding` separately models desired/applied resources. A new request invalidates earlier callbacks. This additional hardening is testable without treating it as the historical carry-bug diagnosis.

### State and payload both matter / 状态与内容都要比较

`HeadPresenter` chooses the highest-priority active text and bubble rules separately. Then it resolves the current payload. An item change can update the icon without restarting an unchanged animation loop.

`Attach` clears cached presentation; `Refresh` reconstructs it from live data. An engine implementation also needs delegate cleanup, late component/resource initialization and localization refresh. The original shared framework supports these concerns; this portable sample models the state-reconstruction boundary, not every engine hook.

### Intent is not authority / 操作意图不同于权威结果

The player-management UI can report locked slots, full capacity or insufficient power before submitting a request. Those checks improve feedback but do not replace server validation.

`WorkerPanel` re-reads current data before requesting, suppresses duplicate pending clicks, forwards server rejections, and invalidates callbacks when the view is rebound/closed. Acknowledgement is not a fabricated change to production progress.

## Efficiency: what is and is not demonstrated / 效率边界

The original head UI combines event-driven updates with checks for auxiliary payload changes. It should not be described as eliminating all per-frame work. The sample exposes text/icon/animation deltas so an integration need not rebuild every part of a widget on every refresh.

No FPS, draw-call, GPU or memory improvement is claimed. World-space visibility, update frequency and resource-loading costs require profiling in a real scene.

原 UI 结合事件更新和附属数据检查，不是彻底消除 Tick。演示只展示差异更新约束，不宣称获得未经测量的帧率、渲染或内存收益。

## Intentional simplifications / 有意简化

- No production resource accounting or offline simulation.
- No recreation of the complete set of work types.
- No real Actor/GA generation or asset validation.
- No game artwork, private logs or internal source-history identifiers.
- No claim that synthetic test results measure production performance.
