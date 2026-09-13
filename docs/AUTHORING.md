# Authoring workflow and reuse boundaries

[Overview](../README.md) · [Source-grounded code tour](CODE_TOUR.md) · [Architecture](ARCHITECTURE.md)

The production workflow connected a creature Actor Blueprint, work Gameplay Abilities and state-dependent feedback. My contribution included integration and iteration within that workflow. Shared UI framework design and implementation were collaborative.

Original editor screenshots are unavailable. The description below uses actual component/configuration interfaces and the development workflow; it does not substitute a fabricated editor image or claim a verified one-click asset generator.

## Why the workflow is shared

Workers followed an agreed behaviour contract and shared a base-tree workflow. My approach was to reuse that contract for predictable head feedback, varying established work families through task/ability parameters and content. Behaviour signals select the presentation mode; current work data and UI configuration supply its contents. [Decision story, examples and trade-offs](DECISIONS.md).

## 1. Establish the creature and its work behaviour

A creature Actor Blueprint supplies the content-specific entity and connects work reception, controller and presentation components. Work abilities handle receiving, performing, finishing and interruption behaviour. Movement tasks bridge the current job to its destination.

The production-line component exposes fields such as `BodyRadius`, idle movement ranges, `WorkInternal`, `TeleportFxPath`, `EmoteCmtIDMap` and bubble-emote settings. Names follow the actual interface, including existing spellings.

For established work families, task parameters and abilities provide the variation within the shared workflow. This does not imply that a new lifecycle can always be added through configuration alone; specialised behaviour can still require an explicit extension.

## 2. Map runtime meaning to a shared UI vocabulary

For ability-driven cues, the task activates a GA and the ability's status tags feed the head UI. Ability-selection tags and active status tags serve different purposes. Other cues arrive through AI state or interruption data. The shared head base has three distinct mappings:

- `EditorTagData`: gameplay tags to head-UI state.
- `PalAIStateData`: AI state to head-UI state.
- `PalProduceInterruptedData`: interruption reasons to head-UI state.

Separating these inputs avoids making a text widget responsible for interpreting the entire production system. The base also handles active-state selection and lifecycle; content configuration defines what particular states mean visually.

This is the boundary I worked with in feature integration, not a claim that I independently built the shared framework.

## 3. Configure text and bubbles separately

Text configuration uses `FMachineHeadTextStatusData`: localisable text, font/style, animation name and priority.

Bubble configuration uses `FMachineHeadTipsStatusData`: display type, image, emoji text/key and priority. Display types include item-image, emoji and specialised overload/new-work feedback; `NotShow` allows deliberate suppression.

Each channel has its own `EditorData` and priority comparison. Lower numeric values win. The implementation has fallback behaviour for equal-priority additions; it does not establish that an editor validator rejects all conflicts.

The parent head widget already binds text and tips children. The state/configuration selects how these children present information; a behaviour-tree task does not directly instantiate a widget for each work type. The tips channel selects a display mode through its configured `ShowType` and `StateSwitcher`.

Dynamic data matters as much as configuration. A transport bubble resolves the current carried item; interruption text can include a building name. These values can change without changing the broad work state.

## 4. Check combinations and lifecycle

The relevant review questions are:

- Does a work phase map to the intended text and bubble?
- What happens when routine work, an interruption and a gameplay-tag cue coexist?
- Is intentional suppression distinguishable from missing content?
- Does a changed item update the bubble while the state stays the same?
- Can a late-created widget reconstruct current state?
- Do the assigned target and visible behaviour agree?

The source includes state initialisation, event cleanup, payload checks and production diagnostics for these concerns. A proposed test list is not a claim that every combination was historically tested.

## Design trade-off

The shared framework avoids duplicating state collection and widget lifecycle for every work type. Specialised configuration preserves content flexibility. The cost is consistency across multiple mappings, channels and dynamic payloads; adding fields alone cannot guarantee a coherent player experience.

The standalone configuration validator in this repository was written later for demonstration. It is documented in the [reference appendix](REFERENCE_APPENDIX.md), not presented as the original project editor.
