# Appendix: independently written reference code

[Historical case](../README.md) · [Actual-source tour](CODE_TOUR.md) · [Run and verify](TESTING.md)

The main portfolio describes past ProjectZ work. The files below were written separately for this portfolio to make selected concepts executable without the game. **They are not exports, reconstructions with identical behaviour, or original project tools.**

## What each example illustrates

- [WorkModel](../Source/Workers/WorkModel.cpp): explicit assignment identity, work phases and applying accepted data before returning presentation effects.
- [HeadPresenter](../Source/Workers/HeadPresenter.cpp): separate text/bubble priorities and text/icon/animation deltas.
- [WorkerPanel](../Content/Lua/WorkerPanel.lua): a new service/view contract for roster interaction, pending actions and callback lifetime.
- [AuthoringProfile](../Content/Lua/AuthoringProfile.lua): symbolic profile validation and a dry-run integration checklist.
- [Worker profile](../Content/WorkerProfiles/gatherer.lua) and [command-line tool](../tools/author_profile.lua): a small configuration example, not real Actor/GA asset creation.
- [Synthetic trace](../examples/work_trace.cpp): an artificial work cycle for observing the model.

## Differences that must not be projected onto the original

The reference's assignment ID, monotonic revision and callback-generation guards are explicit demonstration policies. The actual production-line component uses current/previous jobs, work types and state transitions.

The reference presenter can update an icon without restarting an unchanged animation. The actual tips-widget refresh calls broader style/animation logic and checks payload in Tick. The reference is not evidence that the production implementation performed the same minimal delta update.

The reference panel's snapshot/subscribe/request interface is invented for the example. Its tests use a fake service, not the game's management-panel API.

The validator checks required fields, supported work type, finite timings, status coverage and priority conflicts. It does not launch Unreal, create Blueprints or Gameplay Abilities, validate asset existence, or render widgets. Profiles are executable Lua; only trusted profiles should be loaded.

## Why keep this material

The examples let a reviewer discuss contracts and run edge cases independently. Their value is explanatory. Historical claims remain anchored in the [source-grounded tour](CODE_TOUR.md), and the [verification chapter](TESTING.md) keeps sample checks separate from real-engine validation.
