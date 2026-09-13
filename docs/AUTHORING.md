# Authoring journey / 配置工作流

[Overview](../README.md) · [Architecture](ARCHITECTURE.md) · [Tests](TESTING.md)

**Goal:** adding a new worker should connect behaviour and feedback deliberately, rather than leave the UI as an afterthought.

**目标：**新增机械兽时，把行为与反馈一起配置，而不是玩法完成以后再零散补 UI。

Original editor screenshots are unavailable. This page describes the authoring workflow; the [gameplay gallery](../media/README.md) shows its player-facing context. The executable profile example below is a newly written reference, not a screenshot or export of the original editor.

原编辑器截图已无法取得。本页保留配置流程说明，[游戏画面](../media/README.md)展示其面向玩家的应用场景。下方可执行配置示例为重新编写的参考实现，不伪装成原编辑器截图或导出内容。

## 1. Establish the creature / 建立机械兽实体

In the production workflow, a creature Actor Blueprint provides the content-specific entity. It must connect to the appropriate work receiver, controller and presentation components.

This is a content-creation/integration step. The case does not claim a verified one-click Actor/GA generator.

在实际配置流程中，机械兽 Actor 蓝图承载具体对象，并接入工作接收、控制器与表现组件。这里描述的是内容创建和接入过程，不宣称存在已经核实的一键生成 Actor/GA 工具。

## 2. Connect the work abilities / 接入工作 GA

Configure the Gameplay Abilities for receiving work, performing it, finishing and interruption/out-of-work behaviour. Movement tasks provide the bridge to a changing destination.

The engineering boundary matters: the work system owns production progress and assignment decisions. A movement or animation callback controls presentation/execution; it should not independently invent successful production.

配置接收工作、工作表演、结束以及中断相关 GA，并通过移动任务连接工作地点变化。生产任务和产出推进属于工作系统；动画或移动回调不能自行创造“生产已成功”的事实。

## 3. Configure operating parameters / 配置工作参数

The native work component exposes parameters such as travel/idle ranges, work intervals, teleport effects and emote-related settings. Content can vary without every difference requiring a new branch in gameplay code.

As more work types are introduced, the important review is which parameters are genuinely shared and which belong to a specific work type.

原生工作组件开放移动/闲逛范围、工作间隔、传送效果和表情相关参数。增加内容时，需要判断哪些参数通用，哪些只属于某种工作，不是把所有差异都塞进一个巨大通用配置。

## 4. Author world-space feedback / 配置头顶状态反馈

The shared head UI uses state-to-presentation configuration. Text entries include localized text, style, animation and priority. Bubble entries can choose item images, emoji-style feedback or specialised conditions such as overload.

For example, gathering needs its work message; transporting needs the current item's image; missing energy needs a reason that wins over routine work. Text and bubble channels can arbitrate independently. An intentional suppression rule is different from a missing icon.

Dynamic placeholders and payloads also matter. A missing target/building name or changed item cannot be solved solely by changing a state enum.

共享头顶 UI 通过状态配置文字、样式、动画、优先级，以及物品图、表情或特殊气泡类型。采集、搬运和缺能量需要不同反馈；文字与气泡可以分别仲裁。“主动隐藏气泡”也不同于“忘记配置图标”。

状态之外的具体数据同样重要：目标建筑名称、搬运物品和中断原因，可能在状态不变时发生变化。

## 5. Exercise combinations and inspect execution / 组合测试与执行调试

The project has targeted head-state add/remove controls, plus production/movement diagnostics. Use them for different questions:

- **Authoring:** does the state select the intended text, image and animation?
- **Arbitration:** what happens when work information and an urgent blocker coexist?
- **Payload:** does the item/building change refresh the content without replaying the entire state?
- **Lifecycle:** what happens when the widget is constructed after work has already started?
- **Execution:** do the assigned destination, movement target and visible action agree?

项目中已有头顶状态定向增删入口和生产移动调试。配置检查、状态竞争、具体内容刷新、控件生命周期、执行目标一致性应分开验证，避免把所有问题都当作 UI 显示错误。

## Standalone tool in this repository / 本仓库的独立工具

[AuthoringProfile.lua](../Content/Lua/AuthoringProfile.lua) validates a small [worker profile](../Content/WorkerProfiles/gatherer.lua), then [author_profile.lua](../tools/author_profile.lua) prints a dry-run integration checklist:

```sh
lua tools/author_profile.lua Content/WorkerProfiles/gatherer.lua
```

It checks required Actor/ability references, supported work type, finite timeout, status coverage, priority conflicts and missing presentation fields.

**What it does not do:** open Unreal, create assets, verify real asset existence, render UMG or validate a Blueprint graph. References are symbolic. This validator is a new showcase implementation of an authoring contract, not a claim that this exact tool shipped.

The Lua file is executable trusted configuration; do not load untrusted profiles as if they were passive JSON data.

工具检查 Actor/能力引用是否填写、工作类型、超时参数、状态覆盖、优先级冲突和表现字段。它不会运行 Unreal、创建真实资源、验证资源存在性或渲染 UMG。它是为展示重新编写的接入约束示例，不等同于原项目工具。Lua 配置可执行，不能当作安全的非执行数据载入陌生文件。

## Collaboration and reuse boundary / 协作与复用边界

Design defines work meaning and feedback priority; art defines visual treatment and animation; engineering defines state lifetime, data access and safe transitions. A review must connect all three, not merely confirm the fields are populated.

Related creature systems may use similar feedback needs. This case limits its reuse claim to the verified production/head-status mechanisms; it does not assume every husbandry feature uses the same implementation.

策划确定工作的含义与反馈优先级，美术决定视觉与动画，工程定义状态生命周期、数据接口和切换规则。评审必须连接三者。相近的养殖系统可能有类似需求，但本案例不把尚未核实的复用关系写成已实现事实。
