# Mechanical workers: from production jobs to readable feedback

**Evan (Yaxin) Ge · C++ / Lua / UMG · Past ProjectZ development work**

Mechanical creatures gather, transport, craft and maintain the player's base automatically. The engineering challenge was connecting their work to behaviour, world-space status and content configuration, so players could understand both progress and interruption.

I developed and maintained the production-to-behaviour integration, including job transitions, changing destinations and carried-item presentation. My work also included associated UI integration and iteration, and contributing to the worker-configuration workflow. The wider production system and shared head-UI infrastructure were developed collaboratively.

[中文 README](README.zh-CN.md) · [Case study](docs/CASE_STUDIES.md) · [Decisions](docs/DECISIONS.md) · [Authoring](docs/AUTHORING.md) · [Code tour](docs/CODE_TOUR.md) · [Architecture](docs/ARCHITECTURE.md) · [Visuals](media/README.md) · [Verification](docs/TESTING.md)

## How to read this case

This is a retrospective of specific work on ProjectZ, not a proposal for a new feature. The main account follows actual implementation behaviour and verified interface relationships. The [source-grounded code tour](docs/CODE_TOUR.md) retains relevant interface names and explains their connections; implementation bodies and project assets are omitted.

The existing portable code is separately written **reference material**, collected in the [reference appendix](docs/REFERENCE_APPENDIX.md). It is not original game code, and its tests are not historical validation results. Documentation is in English, with one Chinese README.

## Three engineering decisions

**1. Translate work state before triggering behaviour.**

A movement phase is not automatically a new job. The production-line component builds a job description from current worker data, coordinates previous/current work and handles destination changes through a defined restart path. It uses work types and transition state, not the assignment-ID/revision scheme in the portable reference. [Actual work and trade-offs](docs/CASE_STUDIES.md#one-assignment-many-phases).

**2. Keep state meaning and item payload consistent across views.**

A blocker must not silently become idle behaviour, and a carried object must refresh after the new job data is applied. Head text and bubbles also have their own state arbitration and payload checks. “Transporting” can remain true while the item being transported changes. [Consistency story](docs/CASE_STUDIES.md#state-and-payload-consistency).

**3. Reuse the behaviour contract; drive feedback with state and data.**

Workers shared a base behaviour-tree workflow, with established job differences expressed through task parameters, abilities and content. My approach was to let behaviour signals drive predictable head feedback: for ability-led cues, Task → GA → status tags → UI state. The UI selects a configured presentation and fills it with current work data, rather than requiring a different interface for every job. AI-state and interruption mappings remain complementary inputs. [My reasoning and actual implementation boundaries](docs/DECISIONS.md) · [Authoring fields](docs/AUTHORING.md).

## The player experience

Assign workers in the management interface, then read their current work and item context in the world. Facility production overlays provide a related but separate view of production.

![Mechanical workers beside furnaces, with overhead work-phase labels](media/screenshots/world-work-phases.png)

**Focus on the labels above the creatures.** One is tending a furnace; another is moving to fire-tending work. These are creature states, distinct from the facility markers.

Still from [ENFANT TERRIBLE's public gameplay video](https://www.youtube.com/watch?v=QLgoOZu9biw). [Four screenshots with English captions and label translations](media/README.md). The stills show separate states, not a continuous interaction recording or a before/after bug fix.

## Read the implementation in ten minutes

1. [Production-state integration](docs/CODE_TOUR.md#1-production-state-to-current-job): `UPzProductLineComponent`, job data and transition ordering.
2. [Movement and retargeting](docs/CODE_TOUR.md#2-movement-and-retargeting): `UBTT_ProductLineMoveTo` and the destination-change path.
3. [Head-UI state and lifecycle](docs/CODE_TOUR.md#3-head-ui-state-and-lifecycle): `UPzHeadMachineStatusWidgetBase`, initial reconstruction and event cleanup.
4. [Text and bubble specialisation](docs/CODE_TOUR.md#4-text-and-bubble-specialisation): configuration, priority and changing item payload.
5. [Diagnostics](docs/CASE_STUDIES.md#observe-before-guessing): compare the assigned destination, movement state and visible behaviour.

## Scope and verification

The tour describes the actual source relationships without publishing implementation bodies. It is not a complete game module. Original editor captures are unavailable; the authoring chapter uses verified configuration fields and the development workflow, not fabricated editor images.

The [reference appendix](docs/REFERENCE_APPENDIX.md) contains a portable model and independently written tests. No device-performance numbers, rendering gains or full Unreal integration results are claimed. Game visuals remain subject to their respective rights.

**Related cases:** [Building & interactable UI](https://github.com/seak123/building-ui-portfolio) · [Multiplayer, teams & support UI](https://github.com/seak123/multiplayer-ui-portfolio).
