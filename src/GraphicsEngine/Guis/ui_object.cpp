#include <GraphicsEngine/Guis/ui_object.h>

namespace poke {
namespace graphics {
UiObject::UiObject(UiObject* parent, const UiTransform& transform)
    : parent_(parent),
      transform_(transform),
      screenAlpha_(1.0f),
      screenScale_(1.0f)
{
    if (parent_) { parent_->AddChild(this); }
}

UiObject::~UiObject()
{
    if (parent_) { parent_->RemoveChild(this); }

    for (auto& child : children_) { child->parent_ = nullptr; }
}

void UiObject::Update(
    const math::Matrix4 viewMatrix,
    std::vector<UiObject*>& list)
{
    UpdateObject();

    // Transform updates.
    screenTransform_ = transform_;
    screenTransform_.SetAnchor(UiAnchor::kLeftTop);

    if (parent_) {
        screenAlpha_ *= parent_->screenAlpha_;
        screenScale_.Multiply(parent_->screenScale_);

        screenTransform_.GetPosition().Multiply(screenScale_);
    }

    screenTransform_.GetSize().Multiply(screenScale_);

    if (transform_.GetMargins()) {
        if (parent_) {
            math::Vec2 leftTop(transform_.GetAnchor0());

            if (*transform_.GetMargins() & UiMargins::LEFT) {
                leftTop.x = parent_->screenTransform_.GetSize().x * leftTop.x;
            }

            if (*transform_.GetMargins() & UiMargins::TOP) {
                leftTop.y = parent_->screenTransform_.GetSize().y * leftTop.y;
            }

            math::Vec2 rightBottom;

            if (*transform_.GetMargins() & UiMargins::RIGHT) {
                rightBottom.x = parent_->screenTransform_.GetSize().x * (1.0f + transform_.GetAnchor1().x);
            } else {
                rightBottom.x = parent_->screenTransform_.GetSize().x + transform_.GetAnchor1().x;
            }

            if (*transform_.GetMargins() & UiMargins::BOTTOM) {
                rightBottom.y = parent_->screenTransform_.GetSize().y * (1.0f + transform_.GetAnchor1().y);
            } else {
                rightBottom.y = parent_->screenTransform_.GetSize().y + transform_.GetAnchor1().y;
            }

            screenTransform_.SetSize(rightBottom - leftTop);
            screenTransform_.SetPosition(leftTop + parent_->screenTransform_.GetPosition());
        } else {
            screenTransform_.SetSize(transform_.GetAnchor1() - transform_.GetAnchor0());
            screenTransform_.SetPosition(transform_.GetAnchor0());
        }
    } else {
        if (parent_) {
            screenTransform_.SetPosition(screenTransform_.GetPosition() + parent_->screenTransform_.GetSize() * transform_.GetAnchor0());
            screenTransform_.SetPosition(screenTransform_.GetPosition() + parent_->screenTransform_.GetPosition());
        }

        screenTransform_.SetPosition(screenTransform_.GetPosition() - screenTransform_.GetSize() * transform_.GetAnchor1());
    }

    const auto modelMatrix = math::Matrix4::GetWorldMatrix(
        math::Vec3(screenTransform_.GetPosition(), 0.01f * screenTransform_.GetDepth()),
		math::Vec3(),
		math::Vec3(screenTransform_.GetSize()));
    modelView_ = viewMatrix * modelMatrix;
    
    // Adds this object to the list if it is visible.
    if (screenAlpha_ > 0.0f) { list.emplace_back(this); }

    // Update all children objects.

    for (auto child : children_) {
        child->Update(viewMatrix, list);
    }
}

void UiObject::UpdateObject() { }

void UiObject::SetParent(UiObject* parent)
{
    if (parent_) { parent_->RemoveChild(this); }

    if (parent) { parent->AddChild(this); }

    parent_ = parent;
}

void UiObject::AddChild(UiObject* child) { children_.emplace_back(child); }

void UiObject::RemoveChild(UiObject* child)
{
    children_.erase(std::remove(children_.begin(), children_.end(), child), children_.end());
}
} //namespace graphics
} //namespace poke
