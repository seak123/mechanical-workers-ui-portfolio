#include "Workers/WorkModel.h"
#include "Workers/HeadPresenter.h"
#include <iostream>
using namespace workers;
int main() {
    WorkModel work; work.Bind(7);
    HeadPresenter head; head.Attach();
    Snapshot s; s.worker = 7; s.revision = 1;
    auto step = [&](const char* label) {
        auto u = work.Apply(s);
        if (!u.accepted) { std::cerr << "invalid scenario\n"; return; }
        auto h = head.Refresh(work.Current());
        std::cout << label << " | assignment=" << s.assignment
            << " target=" << s.target << ":" << s.targetRevision
            << " text=" << h.frame.textKey << " bubble=" << h.frame.icon << "\n";
        for (auto effect : u.effects) std::cout << "  " << Name(effect) << "\n";
    };
    std::cout << "SYNTHETIC TRACE - reference model, not a game capture\n";
    step("idle");
    ++s.revision; s.assignment = 81; s.job = Job::Transport; s.phase = Phase::Travel;
    s.target = 9; s.targetRevision = 1; step("assigned");
    ++s.revision; s.phase = Phase::Perform; s.payload = "item.ore"; step("pickup");
    ++s.revision; s.phase = Phase::Travel; s.target = 12; step("delivery");
    ++s.revision; s.payload = "item.wood"; step("same state, new payload");
    ++s.revision; s.phase = Phase::Blocked; s.blocker = Blocker::Energy; step("cannot find energy");
    ++s.revision; s.phase = Phase::Travel; s.blocker = Blocker::None; step("resume");
    ++s.revision; ++s.targetRevision; step("target moved");
    ++s.revision; s.phase = Phase::Finished; step("finished");
}
