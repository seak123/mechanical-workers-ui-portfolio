#include "Workers/WorkModel.h"
#include "Workers/HeadPresenter.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
using namespace workers;
int checks = 0;
void Check(bool ok, const char* name) {
    if (!ok) throw std::runtime_error(name);
    ++checks; std::cout << "PASS " << name << "\n";
}
bool Has(const Update& u, Effect e) {
    return std::find(u.effects.begin(), u.effects.end(), e) != u.effects.end();
}
Snapshot Sample() {
    Snapshot s; s.worker = 7; s.revision = 1; s.assignment = 81;
    s.phase = Phase::Travel; s.job = Job::Transport; s.target = 3;
    return s;
}
int main() {
    try {
        WorkModel model; auto s = Sample();
        Check(!model.Apply(s).accepted, "unbound rejects work");
        model.Bind(7);
        auto first = model.Apply(s);
        Check(first.accepted && Has(first, Effect::Move), "late attach restores movement");
        Check(!Has(first, Effect::JobIntro), "late attach does not replay intro");
        Check(!model.Apply(s).accepted, "duplicate revision ignored");
        auto wrong = s; wrong.worker = 8; ++wrong.revision;
        Check(!model.Apply(wrong).accepted, "other worker rejected");
        ++s.revision; s.phase = Phase::Perform; s.payload = "ore";
        auto pickup = model.Apply(s);
        Check(Has(pickup, Effect::Perform) && !Has(pickup, Effect::JobIntro), "pickup is same job");
        Check(Has(pickup, Effect::RefreshProp) && model.Current().payload == "ore",
            "new descriptor committed before carry visual notification");
        ++s.revision; s.phase = Phase::Travel; s.target = 4;
        auto delivery = model.Apply(s);
        Check(Has(delivery, Effect::Move) && !Has(delivery, Effect::JobIntro), "delivery preserves job identity");
        ++s.revision; s.payload = "wood";
        auto item = model.Apply(s);
        Check(Has(item, Effect::RefreshProp), "payload changes without enum change");
        Check(!Has(item, Effect::Move) && !Has(item, Effect::JobIntro), "payload refresh does not restart movement");
        ++s.revision; ++s.targetRevision;
        auto target = model.Apply(s);
        Check(Has(target, Effect::StopAction) && Has(target, Effect::Move), "moving target retargets action");
        Check(!Has(target, Effect::JobIntro), "retarget is not a new assignment");
        ++s.revision; s.phase = Phase::Blocked; s.blocker = Blocker::Energy;
        auto blocked = model.Apply(s);
        Check(Has(blocked, Effect::StopAction) && Has(blocked, Effect::ClearProp), "energy blocker stops work visuals");
        ++s.revision; s.phase = Phase::Travel; s.blocker = Blocker::None;
        auto resumed = model.Apply(s);
        Check(Has(resumed, Effect::Move) && !Has(resumed, Effect::JobIntro), "resume avoids repeated intro");
        ++s.revision; s.assignment = 82;
        auto replaced = model.Apply(s);
        Check(Has(replaced, Effect::JobIntro) && !Has(replaced, Effect::Complete), "replacement is not completion");
        ++s.revision; s.phase = Phase::Finished;
        Check(Has(model.Apply(s), Effect::Complete), "explicit finish plays completion");
        ++s.revision;
        Check(!Has(model.Apply(s), Effect::Complete), "completion not repeated");
        ++s.revision; s.phase = Phase::Idle; s.assignment = 0;
        Check(model.Apply(s).accepted, "return to idle");
        ++s.revision; s.phase = Phase::Travel; s.assignment = 90;
        Check(Has(model.Apply(s), Effect::JobIntro), "fresh assignment plays intro");
        auto invalid = s; ++invalid.revision; invalid.phase = Phase::Blocked;
        Check(!model.Apply(invalid).accepted, "blocked needs explicit reason");
        invalid = s; ++invalid.revision; invalid.phase = Phase::Idle;
        Check(!model.Apply(invalid).accepted, "idle cannot retain assignment");
        model.Unbind();
        Check(!model.HasSnapshot() && !model.Apply(s).accepted, "teardown removes active binding");

        PropBinding prop;
        auto oldTicket = prop.Request("ore");
        auto newTicket = prop.Request("wood");
        Check(!prop.Complete(oldTicket, "ore"), "superseded async prop rejected");
        Check(!prop.Complete(newTicket, "ore"), "matching ticket needs matching payload");
        Check(prop.Complete(newTicket, "wood") && prop.Applied() == "wood", "current prop applied");
        Check(prop.Request("wood") == newTicket, "unchanged desired prop deduplicated");
        prop.Reset();
        Check(prop.Applied().empty() && !prop.Complete(newTicket, "wood"), "teardown invalidates load callbacks");

        HeadPresenter head; s = Sample(); s.payload = "ore";
        Check(!head.Refresh(s).frame.visible, "detached head stays hidden");
        head.Attach();
        auto h = head.Refresh(s);
        Check(h.frame.textKey == "worker.transporting" && h.frame.icon == "ore", "carrying beats generic working");
        Check(h.animationChanged, "first visible state starts animation");
        h = head.Refresh(s);
        Check(!h.textChanged && !h.iconChanged && !h.animationChanged, "unchanged refresh produces no UI delta");
        s.payload = "wood"; h = head.Refresh(s);
        Check(h.iconChanged && !h.animationChanged, "new carried item updates bubble without restarting loop");
        s.phase = Phase::Blocked; s.blocker = Blocker::Energy;
        h = head.Refresh(s, true);
        Check(h.frame.textKey == "worker.needs_energy" && h.frame.icon == "energy",
            "energy blocker outranks overload");
        s.blocker = Blocker::Storage;
        Check(head.Refresh(s).frame.textKey == "worker.storage_missing", "missing storage is explained");
        s.blocker = Blocker::Motivation;
        Check(head.Refresh(s).frame.textKey == "worker.needs_rest", "motivation blocker is explained");
        s = Sample();
        Check(head.Refresh(s, true).frame.icon == "load", "overload competes with normal work");
        head.Detach(); head.Attach(); s.payload = "seed";
        Check(head.Refresh(s).frame.icon == "seed", "reattach reconstructs from current payload");
        auto rules = DefaultRules(); rules.front().suppressBubble = true;
        HeadPresenter quiet(rules); quiet.Attach();
        s.phase = Phase::Blocked; s.blocker = Blocker::Energy;
        Check(quiet.Refresh(s, true).frame.icon.empty(), "suppression wins instead of revealing lower-priority bubble");
        rules = DefaultRules(); rules.back().textPriority = rules.front().textPriority;
        Check(!ValidateRules(rules).empty(), "ambiguous priorities rejected");
        rules = DefaultRules(); rules.back().textKey.clear();
        Check(!ValidateRules(rules).empty(), "missing localization rejected");
        rules = DefaultRules(); rules.back().animation.clear();
        Check(!ValidateRules(rules).empty(), "missing visible animation rejected");
        rules = DefaultRules(); rules.pop_back();
        Check(!ValidateRules(rules).empty(), "incomplete state coverage rejected");
        rules = DefaultRules(); rules.back().status = static_cast<Status>(999);
        Check(!ValidateRules(rules).empty(), "unknown status cannot replace a required state");
        rules = DefaultRules(); rules[0].bubblePriority = 80; rules[3].bubblePriority = 10;
        HeadPresenter independent(rules); independent.Attach();
        s = Sample(); s.phase = Phase::Blocked; s.blocker = Blocker::Energy;
        const auto split = independent.Refresh(s, true).frame;
        Check(split.textKey == "worker.needs_energy" && split.icon == "load",
            "text and bubble arbitrate independently");
        s = {}; h = head.Refresh(s);
        Check(!h.frame.visible && h.frame.icon.empty(), "invalid live state clears stale UI");
        std::cout << checks << " checks passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL " << e.what() << "\n"; return 1;
    }
}
