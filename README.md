# Mechanical workers: from production jobs to readable feedback

![Mechanical workers beside furnaces with overhead work-phase labels.](media/screenshots/world-work-phases.png)

Automatic production becomes readable through world-space status, item bubbles and feedback that follows the worker's current behaviour.

**Evan (Yaxin) Ge · C++ / Lua / UMG · ProjectZ**

[中文 README](README.zh-CN.md) · [Screenshots](media/README.md) · [Explore the work](#deeper-reading)

*Focus on the labels above the creatures: one is tending a furnace; another is moving to fire-tending work. These creature states are distinct from facility markers. [Footage credit and English label guide](media/README.md).*

## My work

I developed and maintained the connection between production jobs, mechanical-worker behaviour and associated UI. This included job transitions, destination changes, carried-item presentation, head-feedback integration and iteration, and the worker-configuration workflow.

The case follows how gameplay state reaches readable feedback, and how common configuration supports different work types.

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
