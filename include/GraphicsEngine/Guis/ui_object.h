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
// Date : 20.03.2020
//-----------------------------------------------------------------------------
#pragma once

#include <Math/vector.h>
#include <Math/matrix.h>
#include <GraphicsEngine/Guis/ui_transform.h>

namespace poke {
namespace graphics {
/**
 * @brief A representation of a object this is rendered to a screen. This object is contained in a parent and has children.
 * The screen object has a few values that allow for it to be positioned and scaled, along with other variables that are used when rendering.
 * This class can be extended to create a representation for GUI images, fonts, etc.
 */
class UiObject {
public:
    UiObject(UiObject* parent, const UiTransform& transform);

    virtual ~UiObject();

    void Update(math::Matrix4 viewMatrix, std::vector<UiObject*>& list);

    virtual void UpdateObject();

    UiObject* GetParent() const { return parent_; }

    void SetParent(UiObject* parent);

    const std::vector<UiObject*>& GetChildren() const { return children_; }

    /**
     * Adds a child from this object.
     * @param child The child to add.
     */
    void AddChild(UiObject* child);

    /**
     * Removes a child from this object.
     * @param child The child to remove.
     */
    void RemoveChild(UiObject* child);

    void ClearChildren() { children_.clear(); }


    UiTransform& GetTransform() { return transform_; }
    const UiTransform& GetTransform() const { return transform_; }
    void SetTransform(const UiTransform& transform) { transform_ = transform; }


    const std::experimental::optional<math::Vec4>& GetScissor() const { return scissor_; }
    void SetScissor(const std::experimental::optional<math::Vec4>& scissor) { scissor_ = scissor; }


    const UiTransform& GetScreenTransform() const { return screenTransform_; }
    math::Matrix4 GetModelView() const { return modelView_; }
    float GetScreenAlpha() const { return screenAlpha_; }
    math::Vec2 GetScreenScale() const { return screenScale_; }

private:
    UiObject* parent_;
    std::vector<UiObject*> children_;

    UiTransform transform_;
    std::experimental::optional<math::Vec4> scissor_;

    UiTransform screenTransform_;
    math::Matrix4 modelView_;
    float screenAlpha_;
    math::Vec2 screenScale_;
};
} //namespace graphics
} //namespace poke
