# Source-grounded code tour

[Overview](../README.md) · [Architecture](ARCHITECTURE.md) · [Authoring](AUTHORING.md) · [Verification](TESTING.md)

This is a reading guide to actual implementation relationships. Interface names and relative module locations were checked against the project code. **Implementation bodies, complete declarations and binary assets are omitted.** The names below are navigation context, not links to unpublished source or a replacement implementation.

## 1. Production state to current job

Module: `Source/ProjectZ/GameLogic/AI/Component/PzProductLineComponent`.

`UPzProductLineComponent` subscribes to worker AI-state changes. In `OnPalStateChanged`, it reads current destination, carried item, charging context and work parameters from the worker component and constructs `FJobDetailData`.

Relevant interfaces:

- `ReceiveProductJob(const FJobDetailData& DetailData)`
- `FinishCurrentJob(const FJobDetailData& NextJobData)`
- `AbortCurrentJob()`
- `ReExecuteLastJob()`
- `RefreshAvatarAppearence()` — existing spelling retained.
- `RefreshCarryItems()`

The job description includes `JobType`, `GoalLocation`, `GoalActor`, `MoveLimitTime`, `AnimateRate`, `CarryItemID`, `ChargingPileGUID` and `ChargingSlotIndex`.

State branches determine the work family; the common tail handles a valid new job and refreshes appearance after applying the job transition. `RefreshAvatarAppearence` compares `CurrentJob.CarryItemID` with `CarryingItemID` before calling `RefreshCarryItems`.

Blocked states are not all equivalent. In the inspected implementation, `LackOfEnergy` maps to `LackMotivation`, while overload/unavailable-charging branches map to `NoEnergy`. The case therefore does not simplify every energy-related condition into a single idle or no-energy state.

## 2. Movement and retargeting

Module: `Source/ProjectZ/GameLogic/AI/Tasks/BP2CPP/BTT_ProductLineMoveTo`.

`UBTT_ProductLineMoveTo::ExecuteTask` reads the current job and destination from the production-line component. For busy work it calls `AIPalMoveToWork` with the target and movement parameters and waits for the controller's completion callback. `OnEndTask` finishes the behaviour-tree task and unbinds that completion path.

Within the component, `OnDestinationChanged` aborts busy work, updates the remembered destination from the worker component and calls `ReExecuteLastJob`. This is a defined retargeting path, not proof of a separate assignment-identity system.

Receive/finish animation flags are queried and reset through the `NeedPlay*JobAnim` and `ResetNeedPlay*JobAnim` interfaces. Content-specific ability implementations are not reproduced here.

## 3. Head UI state and lifecycle

Module: `Source/ProjectZ/GameLogic/UI/Head/Machine/WidgetBase/PzHeadMachineStatusWidgetBase`.

`UPzHeadMachineStatusWidgetBase` owns:

- `EditorTagData`: gameplay-tag to UI-state mapping.
- `PalAIStateData`: AI-state to UI-state mapping.
- `PalProduceInterruptedData`: interruption-reason to UI-state mapping.
- `ActiveStatusCache`, `CurStatus` and `CurStatusDataCache`: active states, chosen state and auxiliary payload cache.

Follow these relationships:

1. `InitData` binds the owning creature, registers tag/component hooks, initialises configuration and state, and registers the widget with the head-widget manager.
2. `InitPalComp` subscribes to AI-state, load, interruption and other relevant changes.
3. `InitState` reconstructs applicable state from current component data and owned tags.
4. `AddNewStatus` / `RemoveStatus` maintain the active set and choose visible state using subclass priority rules.
5. `OnStatusChanged` caches payload data; `NativeTick` checks for changes to that payload even when the enum is unchanged.
6. `OnResLoadFinish`, `OnPalComponentInit` and `OnCultureChanged` cover additional initialisation/refresh paths.
7. `NativeDestruct` → `RemoveAllBind` releases component/tag/localisation bindings and manager registration.

The base checks payload changes; it does not compute production progress.

## 4. Text and bubble specialisation

Modules: `PzHeadMachineTextStatus` and `PzHeadMachineTipsStatus`, under `Source/ProjectZ/GameLogic/UI/Head/Machine`.

**Text:** `FMachineHeadTextStatusData` contains `StatusText`, `StatusTextFont`, `StatusAnim` and `Priority`. `UPzHeadMachineTextStatus` uses its own `EditorData` map, resolves configured animation/text and handles dynamic text context.

**Bubble:** `FMachineHeadTipsStatusData` contains `ShowType`, `StatusImage`, `EmojiText`, `EmojiIconKey` and `Priority`. `UPzHeadMachineTipsStatus` supports item-image, emoji and specialised display modes, including intentional suppression.

For dynamic content, follow:

`SetStatusCacheData` → `GetAndSetStatusDataOnChanged` → `OnStatusDataChanged` → `RefreshStyle`.

`GetStateIconPath` can resolve the current carried item or another context-specific payload. A changed carried item therefore need not wait for a new AI-state enum before its bubble is refreshed. Style refresh is broader than an icon-only update; animation logic is part of that path.

## 5. Configuration and diagnostics

The production-line component exposes work ranges, work interval, teleport-effect and emote settings for content authoring. Its emote path can update gameplay tags used by the shared head UI.

`FProductLineDebugData` holds work type, assigned destination, blackboard/movement/teleport destinations, old/new AI state, worker identity and controller information. `DrawCurrentJobDebug` and `NeedDrawDebug` support inspecting this integration.

These are actual authoring/debug interfaces. The standalone validator and synthetic trace elsewhere in this repository are separately written examples, not the original tools.

## Reference-code boundary

[REFERENCE_APPENDIX.md](REFERENCE_APPENDIX.md) maps the portable model to the concepts it illustrates and calls out additional safeguards it introduces. It should be read after this tour, not used to infer exact historical implementation.
