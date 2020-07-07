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
// Date : 08.12.19
//-----------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>

namespace poke {
namespace graphics {
class Instance;
class PhysicalDevice;
class Window;

/**
 * \brief Encapsulate VkSurfaceKHR. It's a class to make a connection between Vulkan and the window
 */
class Surface {
public:
    /**
     * \brief 
     * \param instance 
     * \param physicalDevice 
     * \param window 
     */
    Surface(
        const Instance& instance,
        const PhysicalDevice& physicalDevice,
        Window& window);
    ~Surface();

    operator const VkSurfaceKHR &() const { return surface_; }
    const VkSurfaceKHR& GetSurface() const { return surface_; }

    const VkSurfaceCapabilitiesKHR& GetCapabilities() const;

    const VkSurfaceFormatKHR& GetFormat() const { return format_; }
private:
    const Instance& kInstance_;
    const PhysicalDevice& kPhysicalDevice_;

    VkSurfaceKHR surface_;
    VkSurfaceCapabilitiesKHR capabilities_;
    VkSurfaceFormatKHR format_;
};
} //namespace graphics 
} //namespace poke
