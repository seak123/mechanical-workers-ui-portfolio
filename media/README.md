# In-game UI gallery

[Overview](../README.md) · [Engineering stories](../docs/CASE_STUDIES.md) · [Authoring workflow](../docs/AUTHORING.md)

Four stills from [ENFANT TERRIBLE's public YouTube gameplay video](https://www.youtube.com/watch?v=QLgoOZu9biw), supplied for this case study by Evan Ge. The recording is by the video creator, not Evan. Creator watermarks and the supplied images are preserved unchanged. Exact timestamps are not available.

Each figure is an independently labelled visual state, not a claimed chronological sequence. English UI translations below are descriptive, not official localisation. These images illustrate the feature; they do not by themselves prove implementation details or a before/after bug fix.

## 1. Production roster

![Production team on the left, reserve creatures in the centre, and home energy along the bottom](screenshots/production-roster.png)

> The management screen brings together the active production team, reserve creatures and home energy, giving the player context for assigning workers.

**What to notice:** the left column shows the production roster and its capacity (2/10); the centre shows reserve creatures (8/500). The home-energy readout is visible along the bottom. The unselected details area and the creature card over the roster show a selection/assignment context, but this still does not demonstrate a completed drag-and-drop operation.

**Key labels:** 生产编队 — Production team; 后备玛卡 — Reserve creatures; 家园动力 — Home energy; 尚未选中玛卡 — No creature selected.

**Engineering connection:** [management UI boundary](../docs/ARCHITECTURE.md#management-ui-boundary). The screenshot provides assignment context; the portable reference panel is not the historical management implementation.

## 2. Work phases in the world

![Mechanical creatures beside furnaces with overhead labels for tending fire and moving to fire-tending work](screenshots/world-work-phases.png)

> World-space labels distinguish a worker tending a furnace from one moving to fire-tending work, keeping the current work phase visible beside the creature.

**What to notice:** workers near the furnaces have different activity labels. The player can read those labels in the world without opening the management panel. The creature text and the separate markers above the facilities are different UI elements.

**Key labels:** 控火中 — Tending fire; 控火移动中 — Moving to fire-tending work.

**Engineering connection:** [one assignment, many phases](../docs/CASE_STUDIES.md#one-assignment-many-phases). The image illustrates distinct phases; the code/story explains how they are coordinated.

## 3. Facility production state

![A furnace overlay showing Production paused, a remaining count, a worker portrait and a materials panel](screenshots/facility-production-paused.png)

> The facility overlay combines a paused production state, a remaining count, worker context and material information at the workstation.

**What to notice:** the large red message makes the paused state prominent. A worker portrait and the nearby material panel provide related context. The picture alone does not establish why production is paused, so it is not presented as evidence of the energy-related bug described elsewhere.

**Key labels:** 生产暂停 — Production paused; 剩余：2 — Remaining: 2; 添加材料 — Add materials; 木材 — Wood.

**Engineering connection:** [state and payload consistency](../docs/CASE_STUDIES.md#state-and-payload-consistency). Worker state and facility progress are related views, not interchangeable sources of truth.

## 4. Item-specific transport bubble

![A transporting mechanical creature with a wood icon in its overhead bubble](screenshots/transport-item-bubble.png)

> An item-specific overhead bubble identifies wood while the worker's status reads Transporting, making the current work payload visible in the world.

**What to notice:** the bubble communicates which item is involved, while the text communicates the activity. A generic work icon would lose this distinction. This still shows one payload; it does not demonstrate switching items or synchronisation correctness over time.

**Key label:** 搬运中 — Transporting.

**Engineering connection:** [same-state payload refresh](../docs/ARCHITECTURE.md#state-and-payload-both-matter) and the carry-visual ordering story.

## Authoring and diagnostics

Original editor captures are unavailable. The [authoring journey](../docs/AUTHORING.md) explains Actor Blueprint setup, work abilities and head-status configuration without substituting a fabricated editor screenshot. The [source-grounded tour](../docs/CODE_TOUR.md) explains actual interfaces with implementation bodies omitted. The separate configuration example, validator and synthetic trace are documented in the [reference appendix](../docs/REFERENCE_APPENDIX.md).
