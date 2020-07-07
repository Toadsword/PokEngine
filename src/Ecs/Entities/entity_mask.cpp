#include <Ecs/Entities/entity_mask.h>

#include <cmath>
#include <bitset>

namespace poke::ecs {
void EntityMask::AddComponent(const ComponentMask attribute)
{
    componentMask_ |= attribute;
}

void EntityMask::RemoveComponent(const ComponentMask attribute)
{
    componentMask_ &= ~attribute;
}

bool EntityMask::HasComponent(const ComponentMask attribute) const
{
    ComponentMask componentCheck = ComponentType::EMPTY;
    componentCheck |= attribute;
    return (componentMask_ & componentCheck) == componentCheck;
}

bool EntityMask::HasSameComponents(const ComponentMask componentMask) const
{
    return HasComponent(static_cast<ComponentType::ComponentType>(componentMask));
}

ComponentMask EntityMask::GetComponentMask() const { return componentMask_; }

std::vector<ComponentType::ComponentType> EntityMask::GetComponentsByType() {
	std::vector<ComponentType::ComponentType> componentsType;
	componentsType.reserve(std::log2(static_cast<int>(ComponentType::LENGTH)));

	return std::vector<ComponentType::ComponentType>();
}


void EntityMask::Clear()
{
    componentMask_ = static_cast<unsigned int>(ComponentType::EMPTY);
}

void EntityMask::SetActive() { AddComponent(EntityFlag::IS_ACTIVE); }

bool EntityMask::IsActive() const
{
    return HasComponent(static_cast<ComponentType::ComponentType>(EntityFlag::IS_ACTIVE));
}

bool EntityMask::IsGraphicsActive() const
{
    return HasComponent(static_cast<ComponentType::ComponentType>(EntityFlag::IS_VISIBLE));
}

EntityTag EntityMask::GetTag() const
{
    std::bitset<kFlagLength> tagFlag;
    for (int i = 0; i < kFlagLength; i++) {
        tagFlag.set(
            i,
            componentMask_ >> (static_cast<int>(std::log2(
                static_cast<int>(EntityFlag::TAG_START))) + i) & 1);
    }

    return static_cast<EntityTag>(tagFlag.to_ulong());
}

void EntityMask::SetTag(const EntityTag tag)
{
    if (tag > kMaxFlagValue) { return; }

    const std::bitset<kFlagLength> tagFlag{
        static_cast<unsigned long long>(tag)
    };

    for (int i = 0; i < kFlagLength; i++) {
        componentMask_ ^= (- tagFlag.test(i) ^ componentMask_) & 1u <<
            (static_cast<int>(std::log2(
                static_cast<int>(EntityFlag::TAG_START))) + i);
    }
}

bool EntityMask::IsEmpty() const { return componentMask_ == 0; }

} //namespace ecs::poke
