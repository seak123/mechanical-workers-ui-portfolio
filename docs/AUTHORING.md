# Authoring workflow and reuse boundaries

[Overview](../README.md) · [Source-grounded code tour](CODE_TOUR.md) · [Architecture](ARCHITECTURE.md)

The production workflow connected a creature Actor Blueprint, work Gameplay Abilities and state-dependent feedback. My contribution included integration and iteration within that workflow. Shared UI framework design and implementation were collaborative.

Original editor screenshots are unavailable. The description below uses actual component/configuration interfaces and the development workflow; it does not substitute a fabricated editor image or claim a verified one-click asset generator.

## 1. Establish the creature and its work behaviour

A creature Actor Blueprint supplies the content-specific entity and connects work reception, controller and presentation components. Work abilities handle receiving, performing, finishing and interruption behaviour. Movement tasks bridge the current job to its destination.

The production-line component exposes fields such as `BodyRadius`, idle movement ranges, `WorkInternal`, `TeleportFxPath`, `EmoteCmtIDMap` and bubble-emote settings. Names follow the actual interface, including existing spellings.

This supports variation through content configuration, but not every work type is just a different parameter set. Work-specific behaviour remains in specialised integration or abilities.

## 2. Map runtime meaning to a shared UI vocabulary

The shared head base has three distinct mappings:

- `EditorTagData`: gameplay tags to head-UI state.
- `PalAIStateData`: AI state to head-UI state.
- `PalProduceInterruptedData`: interruption reasons to head-UI state.

Separating these inputs avoids making a text widget responsible for interpreting the entire production system. The base also handles active-state selection and lifecycle; content configuration defines what particular states mean visually.

This is the boundary I worked with in feature integration, not a claim that I independently built the shared framework.

## 3. Configure text and bubbles separately

Text configuration uses `FMachineHeadTextStatusData`: localisable text, font/style, animation name and priority.

Bubble configuration uses `FMachineHeadTipsStatusData`: display type, image, emoji text/key and priority. Display types include item-image, emoji and specialised overload/new-work feedback; `NotShow` allows deliberate suppression.

Each channel has its own `EditorData` and priority comparison. Lower numeric values win. The implementation has fallback behaviour for equal-priority additions; it does not establish that an editor validator rejects all conflicts.

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
