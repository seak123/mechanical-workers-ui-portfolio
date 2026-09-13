# Engineering stories

[Overview](../README.md) · [Authoring](AUTHORING.md) · [Source-grounded code tour](CODE_TOUR.md)

These are accounts of actual development and maintenance work. Interface relationships follow the original implementation; method bodies are omitted. The separately written portable code is supporting material in the [reference appendix](REFERENCE_APPENDIX.md).

<a id="one-assignment-many-phases"></a>
## 1. One assignment, many phases

### Context and problem

The production system chooses work for mechanical creatures. Movement, performance and transport are related phases, while destination changes or interruption can occur before the cycle finishes. Treating every AI-state transition as an unrelated new job can repeat receive-job cues and make finish behaviour inconsistent.

### My work

I worked on the component translating production state into the creature's current job and presentation. The transition path builds a job description from the current worker data, chooses the work family and handles job transition in a common place.

That description includes target, destination and relevant payload. The component tracks current/previous work and exposes explicit receive/finish cue flags. Transport phases have cue-handling conditions rather than automatically announcing every transition as new work.

For a changed destination, the integration aborts busy work, updates the remembered target position and re-executes the job. This keeps retargeting in the existing job lifecycle instead of adding unrelated reactions to the movement task.

### Contribution and trade-off

The common transition path provided a clearer place to integrate additional work types and investigate inconsistent cues. It remained a practical state-driven component with specialised branches, not a general task scheduler.

The [actual code tour](CODE_TOUR.md#1-production-state-to-current-job) follows `OnPalStateChanged`, `ReceiveProductJob`, `FinishCurrentJob` and `OnDestinationChanged`. The portable model's explicit assignment IDs and revisions are not part of this historical claim.

<a id="state-and-payload-consistency"></a>
## 2. The creature and its feedback must describe current work

### Context and problem

An energy-related condition could fall into idle presentation, losing the reason the creature could not work. A different issue involved refreshing the carried object before the incoming job data had been applied.

Both problems crossed the state-to-presentation boundary. Changing the final label alone would not correct stale job data or the visible carried object.

### My work

I maintained the blocked-state mapping and the order in which job data and carried-item presentation were updated. The current transition path handles the job before `RefreshAvatarAppearence`; appearance refresh compares the current job's item with the presentation-side item ID.

The blocked-state mapping also preserves distinctions between energy, motivation and other conditions. The [tour](CODE_TOUR.md#1-production-state-to-current-job) records the actual branches rather than claiming that all blocked states map to one generic no-energy state.

The shared head UI addresses a related but separate concern. A worker can remain in a transport state while the current item changes. Its tips widget caches the relevant item/piece ID and checks for payload changes independently of the state enum. I used and iterated with this shared infrastructure as part of feature integration; the full framework was a team implementation.

### Contribution and limits

The ordering fix made carried-item refresh read the current job. The head-UI mechanisms provide a way to refresh changing content without requiring an artificial AI-state change.

This does not mean an icon-only update or zero animation work: the actual tips path invokes `RefreshStyle`, which includes style and animation logic. Nor is it evidence of a measured frame-rate improvement. [Actual state/payload and lifecycle paths](CODE_TOUR.md#3-head-ui-state-and-lifecycle).

## Design decision: extend the shared workflow instead of multiplying interfaces

Different work types followed a common behaviour contract. I used that as the reuse boundary: task/ability signals determine the kind of feedback, while work-specific data and presentation configuration determine the content. The same head interface can explain different jobs without directly coupling each task to a separate widget. [Full decision story and code boundaries](DECISIONS.md).

<a id="observe-before-guessing"></a>
## 3. Observe the chain before changing it

### Context

A worker can receive a valid job but move to the wrong destination, appear idle while production continues, or show stale feedback. A screenshot of the final label cannot distinguish those causes.

### My work

I added and iterated production diagnostics that compare the assigned target with blackboard, movement and teleport destinations, alongside old/new AI state and controller information for the same worker.

The relevant interface is `FProductLineDebugData` with `DrawCurrentJobDebug` and `NeedDrawDebug`. This makes the data-to-movement relationship inspectable before deciding whether to change dispatch, behaviour or UI.

The project also has targeted head-status controls for inspecting specific UI states and combinations. Those are complementary to tracing a real worker's execution; they do not reproduce all gameplay conditions.

### Outcome and limits

The investigation could be directed at the layer where state or destination diverged. The diagnostic interfaces demonstrate investigation tooling, not proof that adding logging alone fixed a particular defect.

The optional [synthetic trace](../examples/work_trace.cpp) is a separately written demonstration. It is neither a live multiplayer capture nor the original visual debugger.
