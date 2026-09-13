#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace workers {
enum class Job { Gather, Transport, Craft, Water, Repair };
enum class Phase { Idle, Travel, Perform, Blocked, Finished };
enum class Blocker { None, Energy, Storage, Motivation };

// Integration contract, not a wire format. A bound worker's revision is monotonic.
struct Snapshot {
    std::uint64_t worker = 0, revision = 0, assignment = 0;
    Job job = Job::Gather;
    Phase phase = Phase::Idle;
    Blocker blocker = Blocker::None;
    std::uint64_t target = 0, targetRevision = 0;
    std::string payload;
};
enum class Effect { StopAction, JobIntro, Move, Perform, Complete, RefreshProp, ClearProp };
struct Update { bool accepted = false; std::vector<Effect> effects; };

bool IsValid(const Snapshot& snapshot);
const char* Name(Effect effect);

class WorkModel {
public:
    // Binding is a lifecycle boundary. First snapshot restores state without an intro.
    void Bind(std::uint64_t worker);
    void Unbind();
    Update Apply(const Snapshot& next);
    const Snapshot& Current() const { return current_; }
    bool HasSnapshot() const { return initialized_; }
private:
    std::uint64_t worker_ = 0;
    Snapshot current_;
    bool initialized_ = false;
};

// Independent showcase hardening: callbacks from superseded loads cannot win.
// Historical carry-visual issue was refresh ordering, not a proven async race.
class PropBinding {
public:
    std::uint64_t Request(const std::string& resource);
    bool Complete(std::uint64_t ticket, const std::string& resource);
    void Reset();
    const std::string& Desired() const { return desired_; }
    const std::string& Applied() const { return applied_; }
private:
    std::uint64_t generation_ = 0;
    std::string desired_, applied_;
};
}
