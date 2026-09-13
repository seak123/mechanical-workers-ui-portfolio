#include "Workers/HeadPresenter.h"
#include <algorithm>
#include <set>
#include <stdexcept>
#include <utility>

namespace workers {
std::vector<Rule> DefaultRules() {
    return {
        {Status::NoEnergy, 10, 10, "worker.needs_energy", "energy", "warning"},
        {Status::NoStorage, 20, 20, "worker.storage_missing", "storage_error", "warning"},
        {Status::NoMotivation, 30, 30, "worker.needs_rest", "rest", "warning"},
        {Status::Overload, 40, 40, "worker.overloaded", "load", "pulse"},
        {Status::Carrying, 90, 90, "worker.transporting", "parcel", "work_loop", true},
        {Status::Working, 100, 100, "worker.working", "tools", "work_loop"}
    };
}
std::vector<std::string> ValidateRules(const std::vector<Rule>& rules) {
    std::vector<std::string> errors;
    std::set<Status> states;
    std::set<int> textPriorities, bubblePriorities;
    for (const auto& r : rules) {
        if (!states.insert(r.status).second) errors.push_back("duplicate status");
        if (!textPriorities.insert(r.textPriority).second) errors.push_back("ambiguous text priority");
        if (!bubblePriorities.insert(r.bubblePriority).second) errors.push_back("ambiguous bubble priority");
        if (r.textKey.empty()) errors.push_back("missing localization key");
        if (!r.suppressBubble && r.icon.empty() && !r.payloadIcon)
            errors.push_back("missing bubble resource");
        if (!r.suppressBubble && r.animation.empty()) errors.push_back("missing animation");
    }
    for (auto required : {Status::Working, Status::Carrying, Status::NoEnergy,
        Status::NoStorage, Status::NoMotivation, Status::Overload}) {
        if (states.count(required) == 0) errors.push_back("incomplete status coverage");
    }
    if (states.size() > 6) errors.push_back("unknown status");
    return errors;
}
HeadPresenter::HeadPresenter(std::vector<Rule> rules) : rules_(std::move(rules)) {
    if (!ValidateRules(rules_).empty()) throw std::invalid_argument("invalid head-status rules");
}
void HeadPresenter::Attach() { attached_ = true; frame_ = {}; }
void HeadPresenter::Detach() { attached_ = false; frame_ = {}; }
HeadUpdate HeadPresenter::Refresh(const Snapshot& s, bool overloaded) {
    Frame next;
    if (attached_ && IsValid(s)) {
        std::vector<Status> active;
        if (s.phase == Phase::Blocked) {
            if (s.blocker == Blocker::Energy) active.push_back(Status::NoEnergy);
            if (s.blocker == Blocker::Storage) active.push_back(Status::NoStorage);
            if (s.blocker == Blocker::Motivation) active.push_back(Status::NoMotivation);
        }
        if (overloaded) active.push_back(Status::Overload);
        if (s.phase == Phase::Travel || s.phase == Phase::Perform) {
            active.push_back(Status::Working);
            if (s.job == Job::Transport && !s.payload.empty()) active.push_back(Status::Carrying);
        }
        const Rule* text = nullptr;
        const Rule* bubble = nullptr;
        for (const auto& r : rules_) {
            if (std::find(active.begin(), active.end(), r.status) == active.end()) continue;
            if (!text || r.textPriority < text->textPriority) text = &r;
            if (!bubble || r.bubblePriority < bubble->bubblePriority) bubble = &r;
        }
        if (text) { next.visible = true; next.textKey = text->textKey; }
        // A suppression rule wins arbitration, but intentionally produces no bubble.
        if (bubble && !bubble->suppressBubble) {
            next.icon = bubble->payloadIcon && !s.payload.empty() ? s.payload : bubble->icon;
            next.animation = bubble->animation;
        }
    }
    HeadUpdate out{next, next.textKey != frame_.textKey, next.icon != frame_.icon,
        next.animation != frame_.animation};
    frame_ = next;
    return out;
}
}
