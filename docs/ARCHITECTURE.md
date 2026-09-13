# Architecture

[Overview](../README.md) · [Source-grounded code tour](CODE_TOUR.md) · [Authoring](AUTHORING.md)

## Responsibilities in the actual feature

The production system maintains work and worker data. The behaviour integration and head UI consume that information for different purposes:

- **Production-state integration:** maps worker state, destination and payload to the current presentation job.
- **Movement and abilities:** execute movement and work behaviour, including interruption and retargeting.
- **Head UI:** interprets AI state, gameplay tags, interruption reasons and relevant payload.
- **Management and facility UI:** provide assignment context and production information; neither is interchangeable with creature head status.
- **Authoring and diagnostics:** configure content and inspect consistency across those paths.

The production-line component is not the sole input to all head UI. Both behaviour and UI use worker-component data; gameplay tags provide another connection. A visual animation completing is not proof of authoritative production completion.

## Data and lifecycle boundaries

### Work type is not every movement phase

`FJobDetailData` carries work type, destination, target, timing and payload such as carried-item and charging-slot information. `OnPalStateChanged` translates AI states into this description. Related movement/performance states can map to the same job family; receive/finish cues have explicit control.

The actual integration uses current/previous job and work-state information. An explicit assignment ID and monotonic revision belong to the optional reference model, not this historical data structure.

### Update the job before refreshing its appearance

The state-change path applies job handling before `RefreshAvatarAppearence`. That method compares the current job's carried-item ID with the presentation-side item ID before refreshing the carried object. The ordering prevents presentation from reading the previous job's payload.

This does not prove that every asynchronous resource callback is protected. The reference model's callback-generation guard is additional demonstration logic.

### State and payload both matter

The shared head base selects from active UI states. Text and bubble subclasses supply their own configuration and priority comparisons. A bubble's image may depend on the current carried item, planting target or interruption payload, rather than only the selected UI-state enum.

The base calls `GetAndSetStatusDataOnChanged(CurStatus)` during `NativeTick`. In the tips subclass, a changed payload leads to `OnStatusDataChanged` and `RefreshStyle`. This is a real same-state refresh path, but it is **not** the portable presenter's minimal text/icon/animation-delta algorithm. The production refresh can reset visual styling and invoke animation logic.

### Late creation must reconstruct current state

`InitData`, component-initialisation callbacks and resource-load completion can trigger state initialisation. The base reads current AI state and owned tags, alongside supported auxiliary conditions, rather than depending only on future events.

`NativeDestruct` calls `RemoveAllBind` to remove subscriptions and manager registration. Culture-change handling refreshes presentation for the current state. These lifecycle paths matter independently of which work type the creature performs.

## Trade-offs visible in the implementation

Configuration allows content variation while shared code handles state collection and lifetime. However, configuration must agree across AI-state mappings, tags, interruption reasons and the two presentation channels. A generic base does not eliminate content-validation work.

Lower numeric priority wins in the text and tips implementations. Equal-priority additions have replacement behaviour; that is not equivalent to a tool proving priorities are unique.

Events avoid treating every change as a full UI reconstruction, but the implementation still has payload checks in Tick and timed overload updates. No claim is made that all polling disappeared or that this is a measured renderer optimisation.

## Management UI boundary

The production-roster screenshot shows the player-facing assignment context. It does not establish the exact request or validation implementation. The separately written `WorkerPanel.lua` uses a fake service contract and is documented only in the [reference appendix](REFERENCE_APPENDIX.md).

## What remains outside the case

The scheduler, production accounting, networking, navigation implementation, Gameplay Ability bodies and binary widget/Actor assets are omitted. The [code tour](CODE_TOUR.md) explains verified connections without presenting a runnable copy of the original system.
