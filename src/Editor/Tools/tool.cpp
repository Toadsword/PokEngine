#include <Editor/Tools/tool.h>

namespace poke {
namespace editor {
Tool::Tool(const bool defaultActive) :
    isActive_(defaultActive) {}

void Tool::UpdateActiveState() {
    if (isActive_ && state_ == State::INACTIVE) {
        OnSetActive();

        state_ = State::ACTIVE;
    } else if (!isActive_ && state_ == State::ACTIVE) {
        OnSetInactive();

        state_ = State::INACTIVE;
    } else if (state_ == State::IDLE) {
        if (isActive_) {
            state_ = State::ACTIVE;

            OnSetActive();
        } else { state_ = State::INACTIVE; }
    }
}

void Tool::SetActive(const bool b) { isActive_ = b; }

bool Tool::IsActive() const { return isActive_; }

const char* Tool::GetName() const { return name_.c_str(); }
ToolFlags Tool::GetFlag() const { return toolFlag_; }
} //namespace editor
} //namespace poke
