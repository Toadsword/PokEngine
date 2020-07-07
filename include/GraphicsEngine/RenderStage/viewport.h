//-----------------------------------------------------------------------------
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
// Co-Author :
// Date : 20.01.20
//-----------------------------------------------------------------------------
#pragma once
#include <Math/vector.h>

namespace poke {
namespace graphics {

/**
 * \brief Viewport to draw inside.
 */
class Viewport {
public:
    Viewport();

    explicit Viewport(math::Vec2Int size);

    Viewport(const Viewport& v);

    void SetScale(const math::Vec2 scale) { scale_ = scale; }
    math::Vec2 GetScale() const { return scale_; }

    math::Vec2Int GetSize() const { return size_; }
    bool HasSize() const { return hasSize_; }
    void SetSize(const math::Vec2Int size) { size_ = size; }

    void SetOffset(const math::Vec2 offset) { offset_ = offset; }
    math::Vec2 GetOffset() const { return offset_; }
private:
    math::Vec2 scale_;
    math::Vec2 offset_;
    math::Vec2Int size_;
    bool hasSize_ = false;
};
} //namespace poke
} //namespace graphics
