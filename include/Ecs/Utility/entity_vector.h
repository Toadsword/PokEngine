//----------------------------------------------------------------------------------
// Copyright (c) 2019-2020, POK Family. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of POK Family nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author : Nicolas Schneider
// Date :  16.03.2020
//----------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <algorithm>

#include <Ecs/ecs_utility.h>

namespace poke {
namespace ecs {
/**
 * \brief Sorted vector specifics for using with entities.
 */
class EntityVector {
public:
    explicit EntityVector(const size_t size)
    {
		entities_.reserve(size);
    }

    EntityIndex operator[](const size_t index) {
		return entities_[index];
    }

    bool exist(const EntityIndex entityIndex) const noexcept
    {
		return std::binary_search(entities_.begin(), entities_.end(), entityIndex);
    }

	std::vector<EntityIndex>::iterator find(const EntityIndex entityIndex)
    {
		return std::equal_range(entities_.begin(), entities_.end(), entityIndex).first;
    }

	std::vector<EntityIndex>::iterator insert(const EntityIndex entityIndex)
    {
		return entities_.insert(std::upper_bound(entities_.begin(), entities_.end(), entityIndex), entityIndex);
    }

	std::vector<EntityIndex>::iterator erase(const std::vector<EntityIndex>::const_iterator it)
    {
		return entities_.erase(it);
    }

    std::vector<EntityIndex>::iterator begin()
    {
		return entities_.begin();
    }

	std::vector<EntityIndex>::iterator end()
	{
		return entities_.end();
	}

    size_t size() const noexcept
    {
		return entities_.size();
    }

    size_t capacity() const noexcept
    {
		return entities_.capacity();
    }

    void clear() noexcept
    {
		entities_.clear();
    }

    void reserve(const size_t newCapacity)
    {
		entities_.reserve(newCapacity);
    }

    EntityIndex back() const 
    {
		return entities_.back();
    }

private:
	std::vector<EntityIndex> entities_;
};
} //namespace ecs
} //namespace poke
