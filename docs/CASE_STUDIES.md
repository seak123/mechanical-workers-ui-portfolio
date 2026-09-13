# Engineering stories / 工程案例

[Overview](../README.md) · [Authoring](AUTHORING.md) · [Architecture](ARCHITECTURE.md)

The cases below describe production experience. Linked sample code expresses the underlying contracts in an independently written model; it is not the original implementation.

下文讲述实际开发经历。链接代码用独立演示表达其中的接口约束，并非原实现的拷贝。

<a id="one-assignment-many-phases"></a>
## 1. One assignment, many phases / 一次任务，多个阶段

### Context & problem

The production system chooses work for mechanical creatures. A single assignment can include receiving a job, moving to a work point, performing an action, transporting an item and returning to work. Energy, motivation, interruptions and target changes can affect the cycle.

Treating every low-level AI state change as a new job produces repeated receive-job cues and poorly defined finish behaviour. Treating every position change as a new assignment is equally misleading.

### My approach

I worked on the component that translates production state into the creature's current job and presentation. Rather than repeatedly issuing a job in individual state branches, the flow builds a job description and handles the transition in a common place.

The description carries the work type, target/destination and relevant visual payload. Moving and working can remain part of the same semantic job. Transport pickup and delivery need coordinated cues. Receive/finish cues have explicit control rather than being a side effect of every enum change.

When a working destination changes, the current movement can be aborted and re-executed against the new destination. That is a movement update—not necessarily a new work assignment.

### Contribution & impact

This clarified the integration boundary as work types expanded. It provided one place to reason about previous/current work, target changes and presentation timing, instead of adding unrelated reactions in every state handler.

I would not describe this as a measured performance improvement. The value is behavioural consistency and a more maintainable path for feature growth.

**Sample:** [WorkModel::Apply](../Source/Workers/WorkModel.cpp). The sample uses an explicit assignment identifier and monotonic snapshot revision to express identity and ordering clearly; those are showcase contracts, not claims about the original network protocol.

### 中文讲述

生产系统决定机械兽做什么，但一次工作可能经历接收任务、移动、表演、搬运和返回等多个阶段，还会受到能量、动力、中断和目标变化影响。

我负责的接入层需要把底层工作状态转换成当前任务和角色表现。难点不是增加几个动画，而是确定哪些变化代表新工作，哪些只是同一工作的阶段变化。我参与把各分支中的任务处理收束到共同入口，先形成完整的任务描述，再处理结束、接收和表现更新。搬运的拾取与送达不能各自重复播放“接到新任务”。

目标在工作中发生变化时，则重启相应的移动过程，并使用新目标继续工作。这样新类型可以沿着明确的任务描述和切换路径扩展，而不是继续堆叠状态特例。这项工作的价值是行为一致性和可维护性，不是未经测量的帧率提升。

<a id="state-and-payload-consistency"></a>
## 2. State and payload consistency / 状态与具体内容的一致性

### Case A — An energy failure should not look like idle

An energy-related work failure was grouped with idle states in the presentation mapping. The fix moved it to the no-energy path. A small mapping change matters because it defines whether the creature's behaviour agrees with the reason the system cannot proceed.

The diagnostic question is not only “which label is wrong?” It is “does the authoritative reason survive each translation from work state to action and UI?”

### Case B — The state is unchanged, but the carried item is different

A carry visual could be refreshed before the new job description had been applied. The correction moved the refresh after job processing and explicitly tracked the carried-item identity used by presentation.

The distinction is between **what this job now wants to show** and **what the visual currently represents**. Comparing only an AI state or job enum misses changes from one carried item to another.

The confirmed production issue was update ordering and item identity. The sample's generation-checked resource completion is an additional safeguard; it is not presented as the historical root cause.

### UI implications and collaboration

The shared head-status system supports concurrent conditions, with priorities for text and bubbles. An urgent blocker can override routine work information. Bubble payloads may change while the status stays the same, so the payload must also be checked. Late widget creation must reconstruct from current data, rather than wait for a future state event.

I used and worked with these shared mechanisms as part of the feature's integration and iteration. This is not a claim that I independently built the whole head-widget framework.

### Contribution & impact

My fixes addressed concrete disagreements between current work and visible behaviour. Together with the shared UI contract, they make the production state understandable at the point where the player sees the creature.

**Samples:** [HeadPresenter](../Source/Workers/HeadPresenter.cpp), [PropBinding](../Source/Workers/WorkModel.cpp), and the carry/blocker/reattachment tests.

### 中文讲述

一个实际问题是缺少能量的状态被归入闲置表现。修复本身不大，但它揭示了跨层问题：底层知道为什么不能继续工作，经过任务映射以后，这个原因不能丢失，否则机械兽表现与界面解释就会矛盾。

另一个问题是搬运物件刷新过早，新任务描述还没生效，表现就读取了旧内容。我调整了刷新顺序，并区分当前工作要求显示的物品和表现侧记录的物品。这里不能只看“正在搬运”这个状态，因为状态不变，搬运的东西也可能已经变化。

共同使用的头顶 UI 还需要处理多个状态竞争、文字与气泡的优先级，以及控件晚创建后的状态恢复。我的工作是结合这些机制完成玩法和 UI 的接入与迭代；完整共享框架属于团队协作成果。演示中的异步回调代次保护是额外防护，不冒充当时问题的根因。

<a id="observe-before-guessing"></a>
## 3. Observe the chain before changing it / 先观察整条链路

### Context

A creature can be assigned correctly but move incorrectly, appear idle while still working, or display a stale status. Looking only at the last UI label does not distinguish those causes.

### My work

I added and iterated production debugging around the creature's work and movement. Relevant observations include the assigned target, movement/blackboard destination, teleport destination, old/new AI state and controller type.

The useful comparison is whether these values describe the same creature and current work. Spatial markers make a wrong destination visible; state/controller labels help distinguish dispatch, behaviour execution and presentation problems.

The shared UI also offers targeted head-state add/remove controls. These serve a different purpose: checking a particular status or a combination of statuses without needing to create the full gameplay condition.

### Outcome and limits

This made investigations more focused: identify where state or destination diverges before modifying UI, navigation or ability behaviour. The debug changes are evidence of investigation tooling, not proof that adding logs alone fixed a specific defect.

The [synthetic trace](../examples/work_trace.cpp) demonstrates the inspection contract. It is not a recording of a live multiplayer session or a recreation of the original visual debugger.

### 中文讲述

机械兽可能接到了正确任务，却走向错误的位置；也可能仍在工作，却表现得像闲置。只看最后的气泡文字无法区分这些原因。

我添加、迭代过生产线相关的调试信息，把分配目标、移动与黑板中的目的地、传送目标、新旧 AI 状态以及控制器类型放在一起观察。关键是针对同一只机械兽、同一次工作比较这些值，定位数据在哪里开始不一致。

头顶 UI 的定向状态增删测试则用于检查某种反馈或状态竞争，不必每次完整触发玩法。两套工具各有用途：一套追踪真实执行链，一套检查 UI 状态组合。这里不会把“增加日志”本身描述为某个 bug 已经修复。
