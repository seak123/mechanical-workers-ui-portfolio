#include "Workers/WorkModel.h"

namespace workers {
namespace {
bool Active(Phase phase) { return phase == Phase::Travel || phase == Phase::Perform; }
std::string Prop(const Snapshot& s) {
    return s.job == Job::Transport && Active(s.phase) ? s.payload : std::string{};
}
}
bool IsValid(const Snapshot& s) {
    if (s.worker == 0 || s.revision == 0) return false;
    if (s.phase == Phase::Idle)
        return s.assignment == 0 && s.blocker == Blocker::None;
    if (s.assignment == 0) return false;
    if (s.phase == Phase::Blocked) return s.blocker != Blocker::None;
    return s.blocker == Blocker::None;
}
void WorkModel::Bind(std::uint64_t worker) {
    worker_ = worker; current_ = {}; initialized_ = false;
}
void WorkModel::Unbind() { Bind(0); }
Update WorkModel::Apply(const Snapshot& next) {
    Update out;
    if (worker_ == 0 || next.worker != worker_ || !IsValid(next) ||
        (initialized_ && next.revision <= current_.revision)) return out;
    const auto before = current_;
    const bool first = !initialized_;
    const bool newJob = !first && next.assignment != before.assignment;
    const bool changedKind = !first && next.job != before.job;
    const bool phaseChanged = first || next.phase != before.phase;
    const bool targetChanged = !first && (next.target != before.target ||
        next.targetRevision != before.targetRevision);
    const bool restart = first || newJob || changedKind || phaseChanged || targetChanged;

    // Commit all authoritative fields BEFORE deriving visuals and notifications.
    current_ = next;
    initialized_ = true;
    out.accepted = true;
    if (!first && Active(before.phase) && restart)
        out.effects.push_back(Effect::StopAction);
    if (newJob && Active(next.phase)) out.effects.push_back(Effect::JobIntro);
    if (Active(next.phase) && restart)
        out.effects.push_back(next.phase == Phase::Travel ? Effect::Move : Effect::Perform);
    // A replacement/cancellation is not successful completion.
    if (!first && !newJob && next.phase == Phase::Finished &&
        before.phase != Phase::Finished) out.effects.push_back(Effect::Complete);
    if (first || Prop(before) != Prop(next)) {
        out.effects.push_back(Prop(next).empty() ? Effect::ClearProp : Effect::RefreshProp);
    }
    return out;
}
const char* Name(Effect e) {
    switch (e) {
    case Effect::StopAction: return "stop action";
    case Effect::JobIntro: return "new-job cue";
    case Effect::Move: return "move to current target";
    case Effect::Perform: return "perform current work";
    case Effect::Complete: return "completion cue";
    case Effect::RefreshProp: return "refresh carried prop";
    case Effect::ClearProp: return "clear carried prop";
    }
    return "unknown";
}
std::uint64_t PropBinding::Request(const std::string& resource) {
    if (resource == desired_) return generation_;
    desired_ = resource; applied_.clear();
    return ++generation_;
}
bool PropBinding::Complete(std::uint64_t ticket, const std::string& resource) {
    if (ticket != generation_ || resource.empty() || resource != desired_) return false;
    applied_ = resource; return true;
}
void PropBinding::Reset() { ++generation_; desired_.clear(); applied_.clear(); }
}
