#pragma once
#include "Workers/WorkModel.h"
#include <string>
#include <vector>

namespace workers {
enum class Status { Working, Carrying, NoEnergy, NoStorage, NoMotivation, Overload };
struct Rule {
    Status status;
    int textPriority;
    int bubblePriority; // Lower wins, separately for each channel.
    std::string textKey, icon, animation;
    bool payloadIcon = false;
    bool suppressBubble = false;
};
std::vector<Rule> DefaultRules();
std::vector<std::string> ValidateRules(const std::vector<Rule>& rules);
struct Frame {
    bool visible = false;
    std::string textKey, icon, animation;
    bool operator==(const Frame& r) const {
        return visible == r.visible && textKey == r.textKey &&
            icon == r.icon && animation == r.animation;
    }
};
struct HeadUpdate {
    Frame frame;
    bool textChanged = false, iconChanged = false, animationChanged = false;
};
class HeadPresenter {
public:
    explicit HeadPresenter(std::vector<Rule> rules = DefaultRules());
    void Attach();
    void Detach();
    // Caller provides a coherent live snapshot, including auxiliary item/target data.
    HeadUpdate Refresh(const Snapshot& snapshot, bool overloaded = false);
    const Frame& Current() const { return frame_; }
private:
    bool attached_ = false;
    std::vector<Rule> rules_;
    Frame frame_;
};
}
