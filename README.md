# Mechanical workers: from production jobs to readable feedback

Connect automatic production to player-facing UI: assign a workforce, read each creature's current work, and see which item it is handling.

**Evan (Yaxin) Ge · C++ / Lua / UMG · ProjectZ**

[中文 README](README.zh-CN.md) · [Feature screenshots](#gameplay-screenshots) · [My work](#my-work) · [Decisions](#three-questions-and-decisions) · [Code and tests](#deeper-reading)

## Gameplay screenshots

Three views establish the assignment context and the world-space feedback explored by this case. [All four screenshots, English labels and footage credits](media/README.md).

### Production roster: the player's assignment context

![Management screen with a production team on the left, reserve creatures in the centre and home energy at the bottom.](media/screenshots/production-roster.png)

**What the player sees:** the active **Production team**, **Reserve creatures** and **Home energy** together. The screenshot shows two workers in a team with capacity ten, giving context for workforce selection.

**Connection to the case:** assignments lead into the production-to-behaviour integration below, where worker data becomes a current job. This view introduces the player-facing system before following its runtime behaviour.

[Feature context and management boundary](docs/ARCHITECTURE.md#management-ui-boundary) · [Code: production state to current job](docs/CODE_TOUR.md#1-production-state-to-current-job)

### Head status: distinguish movement from active work

![Mechanical workers beside furnaces with labels for tending fire and moving to fire-tending work.](media/screenshots/world-work-phases.png)

**What the player sees:** labels above the creatures distinguish **Tending fire** from **Moving to fire-tending work**, without opening the management panel. Creature status and facility markers are different UI elements.

**Work behind this view:** job/phase transitions connect to behaviour; ability-driven cues use Task → GA → status tags → head-UI state, alongside AI-state and interruption inputs. Configured text and bubble presentations turn these signals into readable feedback.

[Feature: one assignment, many phases](docs/CASE_STUDIES.md#one-assignment-many-phases) · [Decision: shared behaviour contract](docs/DECISIONS.md) · [Code: head-UI state and lifecycle](docs/CODE_TOUR.md#3-head-ui-state-and-lifecycle)

### Item bubble: explain what the worker is transporting

![A transporting creature with a wood icon in its overhead bubble.](media/screenshots/transport-item-bubble.png)

**What the player sees:** the **Transporting** label explains the activity while the wood icon identifies its payload. The two together communicate more than a generic working indicator.

**Work behind this view:** apply the current job before refreshing carried-object presentation, and keep the broad UI state separate from item-specific content. A new carried item can require refreshed feedback even while the activity stays the same.

[Feature: state and payload consistency](docs/CASE_STUDIES.md#state-and-payload-consistency) · [Code: text and bubble specialisation](docs/CODE_TOUR.md#4-text-and-bubble-specialisation) · [Content configuration](docs/AUTHORING.md)

**Additional view — [facility production paused: screenshot and explanation](media/README.md#3-facility-production-state).** A workstation overlay combines production status, remaining count, worker context and materials. It is a related facility view, distinct from creature head feedback.

**Configuration workflow — [Actor Blueprint, work GA, head-text and bubble setup](docs/AUTHORING.md).** Follow the configuration relationships behind the visible feedback; this chapter uses interface descriptions rather than editor screenshots.

## My work

I developed and maintained the connection between production jobs, mechanical-worker behaviour and associated UI. The work covered runtime integration, visual consistency, head-feedback iteration and the worker-configuration workflow.

- **Job and behaviour integration:** translated worker state into current jobs, coordinated previous/current work, and handled transitions between movement, performance, completion and interruption.
- **Changing destinations:** maintained the retargeting path so updated work destinations propagate into movement and behaviour rather than leaving the worker following an old target.
- **World-space feedback:** worked on the connection between behaviour/ability signals and head text or bubbles, using common state mappings while retaining job-specific content.
- **State and item consistency:** maintained the order of job application and carried-object refresh; treated the activity and its concrete payload as separate inputs to presentation.
- **Content configuration:** contributed to the workflow connecting creature Actor Blueprints, work GAs and head-status settings, including text, images, animation and priority.
- **Diagnostics and iteration:** added and maintained ways to compare assigned work, movement targets, AI transitions and visible behaviour, helping locate the layer where data and presentation diverged.

## Three questions and decisions

### 1. How can different jobs share a predictable feedback system?

Workers followed a common base behaviour-tree workflow, with established job variations expressed through task parameters, abilities and content. I used that contract to connect behaviour to head feedback: for ability-driven cues, a task activates a GA whose status tags feed UI-state mapping.

The UI selects a configured presentation and fills it with current job data. AI states and interruption reasons provide complementary inputs. The task supplies meaning without needing to manipulate individual widget children. [Decision rationale](docs/DECISIONS.md) · [Configuration workflow](docs/AUTHORING.md).

### 2. What changes when the work phase changes but the job does not?

Moving to a work point and performing the work can be phases of the same job. I maintained the production-to-behaviour integration so current worker data is translated into a job description before behaviour is updated.

Previous/current work and an explicit destination-change restart path keep retargeting and phase transitions connected to the assigned work. [Job transitions](docs/CASE_STUDIES.md#one-assignment-many-phases) · [Interface tour](docs/CODE_TOUR.md#1-production-state-to-current-job).

### 3. How does the display stay correct when the status is unchanged?

“Transporting” can remain true while the carried item changes. I maintained the ordering between applying job data and refreshing the carried object, so presentation reads the new job.

Head text and bubbles also resolve state and concrete payload separately. Status and priority choose the feedback; item data supplies what the player sees. This keeps a familiar presentation useful across different jobs and contents. [State and payload consistency](docs/CASE_STUDIES.md#state-and-payload-consistency).

## Outcomes

- Established work types share a feedback vocabulary while retaining job-specific text, imagery and animation configuration.
- Job transitions and destination changes have explicit integration points for behaviour updates and diagnosis.
- Carried-object presentation reads the applied job, and head feedback can refresh its item content even when the broad work state stays the same.

## Deeper reading

- **Feature and architecture:** [Case studies](docs/CASE_STUDIES.md) · [Architecture](docs/ARCHITECTURE.md) · [Decision rationale](docs/DECISIONS.md).
- **Implementation:** [Actual interface and call-relationship tour](docs/CODE_TOUR.md).
- **Content workflow:** [Actor, work-ability and head-feedback configuration](docs/AUTHORING.md).
- **Diagnostics:** [Compare assigned work, movement and visible behaviour](docs/CASE_STUDIES.md#observe-before-guessing).
- **Executable examples:** [Independent reference models — not production code or original editor tools](docs/REFERENCE_APPENDIX.md) · [Reference-model tests](docs/TESTING.md).
- **Evidence scope:** [Historical work, interface descriptions, models and footage](docs/EVIDENCE.md).
- **Visuals:** [Four separately labelled gameplay stills with English captions](media/README.md).

**Related cases:** [Building UI](https://github.com/seak123/building-ui-portfolio) · [Multiplayer UI](https://github.com/seak123/multiplayer-ui-portfolio).
