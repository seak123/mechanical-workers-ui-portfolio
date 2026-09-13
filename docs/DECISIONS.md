# Decision rationale: a shared behaviour contract, data-driven feedback

[Overview](../README.md) · [Authoring workflow](AUTHORING.md) · [Architecture](ARCHITECTURE.md) · [Code tour](CODE_TOUR.md)

## Context: different jobs, an agreed behaviour contract

Mechanical workers had different actions and visual performances, but followed an agreed common behaviour contract. In the workflow I worked with, they shared a base behaviour tree; established job variations were expressed through task parameters, work abilities and job-specific data.

This changed the UI question. I did not need to design an unrelated head-status interface for every creature or job. I needed a predictable connection between the shared behaviour phases and the player's feedback, while preserving the specific meaning of each job.

## My reasoning and contribution

My approach was to let behaviour execution provide the semantic signals for head UI. For ability-driven cues, a behaviour-tree task activates a Gameplay Ability, and the ability's status tags feed the shared head-status system. The UI interprets those signals instead of independently guessing the creature's current action from its animation.

I separated **which kind of feedback to show** from **what that feedback contains**. The behaviour or status selects a configured presentation mode; work-specific data supplies the item, text context and other content, together with the configured visual treatment and animation. I applied this reasoning in the gameplay/UI integration and configuration work, using the team's shared infrastructure.

For an existing work family, the main variation could remain in task parameters, abilities and content configuration. A genuinely new lifecycle or interaction still required an explicit extension; the common contract was not a reason to force every future job into unsuitable generic parameters.

## How that maps to the implementation

The ability-driven route is:

`Behaviour-tree task → activate GA → ability-owned status tags → head-UI state mapping → configured presentation`.

The tag used to **select an ability** and the status tags it owns **while active** have different roles. `UBTTask_ActivateAbility::ExecuteTask` selects abilities using `AbilityTagContainer` and `TryActivateAbilitiesByTag`. `UPzGameplayAbility` provides an optional client-sync path for `ActivationOwnedTags`, controlled by `bSyncTagToClient`; this does not mean all ability tags always replicate.

On the UI side, `UPzHeadMachineStatusWidgetBase` listens for tag-count changes. `EditorTagData` maps relevant tags to UI states, and `OnTagCountChanged` adds or removes those states. Text and bubble subclasses then apply their own configuration and priority rules.

**“The task selects the widget” describes an indirect presentation decision, not widget ownership.** The actual `UPzMachineMonsterWidget` already binds `TextStatus` and `TipsStatus` children. The tips implementation uses `EditorData[Status].ShowType` to select a presentation through `StateSwitcher`; text, images and animation are resolved by the UI configuration and current data. The task does not need a direct reference to each UMG child.

## Concrete example: the same transport mode, a different item

A transport cue tells the player what the worker is doing. The accompanying bubble tells them what it is carrying. The presentation can therefore remain a transport/item-image mode while the payload changes from one item to another.

`GetStateIconPath` reads the current carried-item data. The payload-cache check can trigger `RefreshStyle` even if the broad UI state has not changed. This is why the work/status signal and the concrete display data must remain separate inputs.

The actual refresh includes style and animation logic; this is not a claim that only the icon is updated or that no Tick work remains.

## Reuse boundary and trade-offs

The reusable parts are the behaviour contract, state-to-presentation mapping and head-widget lifecycle. The variable parts are task/ability configuration and job-specific content. That gave me a consistent way to connect new work content to feedback without adding a separate UI implementation for each worker.

The cost is that the contracts must stay aligned. A task can execute correctly while a missing tag mapping or unsuitable priority hides the expected feedback. An ended or interrupted behaviour also needs its status to stop dominating the display. These are the relevant integration checks; no unrecorded test coverage or measured development-time saving is claimed.

GA tags are one route, not the entire system. The inspected framework also maps AI states and production-interruption reasons, and `BTT_PalPlayEmote` can call the production-line component to update emote tags directly. These routes share the same UI vocabulary rather than requiring every cue to originate in a GA.

## Evidence and ownership boundary

The shared-base-tree workflow and the design reasoning above are my account of the work. The native task, ability-tag support, head-state mapping and bound-widget presentation were checked in code. The complete Blueprint tree and per-ability configuration are not reproduced or claimed to have been executed by this portfolio. The shared framework remains a team implementation; my contribution is described through its integration, configuration and iteration, not sole authorship.
