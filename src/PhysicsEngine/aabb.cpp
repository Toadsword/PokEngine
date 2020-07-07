#include <PhysicsEngine/aabb.h>

namespace poke {
namespace physics {

bool AABB::OverlapAABB(const AABB other) const
{
    //Test box
    const float otherMaxX = other.worldPosition.x + other.worldExtent.x * 0.5f;
    const float otherMinX = other.worldPosition.x - other.worldExtent.x * 0.5f;

    const float otherMaxY = other.worldPosition.y + other.worldExtent.y * 0.5f;
    const float otherMinY = other.worldPosition.y - other.worldExtent.y * 0.5f;

    const float otherMaxZ = other.worldPosition.z + other.worldExtent.z * 0.5f;
    const float otherMinZ = other.worldPosition.z - other.worldExtent.z * 0.5f;


    const float maxX = worldPosition.x + worldExtent.x * 0.5f;
    const float minX = worldPosition.x - worldExtent.x * 0.5f;

    const float maxY = worldPosition.y + worldExtent.y * 0.5f;
    const float minY = worldPosition.y - worldExtent.y * 0.5f;

    const float maxZ = worldPosition.z + worldExtent.z * 0.5f;
    const float minZ = worldPosition.z - worldExtent.z * 0.5f;

    //Test other being contained inside this
    //Test on X
    if (maxX >= otherMaxX && otherMaxX >= minX ||
        maxX >= otherMinX && otherMinX >= minX ||
		otherMaxX >= maxX && maxX >= otherMinX ||
		otherMaxX >= minX && minX >= otherMinX) {
        //Test on Y
        if (maxY >= otherMaxY && otherMaxY >= minY ||
            maxY >= otherMinY && otherMinY >= minY ||
			otherMaxY >= maxY && maxY >= otherMinY ||
			otherMaxY >= minY && minY >= otherMinY) {
            //Test on Z
            if (maxZ >= otherMaxZ && otherMaxZ >= minZ ||
                maxZ >= otherMinZ && otherMinZ >= minZ || 
				otherMaxZ >= maxZ && maxZ >= otherMinZ ||
				otherMaxZ >= minZ && minZ >= otherMinZ) { return true; }
        }
    }
    
    return false;
}
} //namespace physics
} //namespace poke