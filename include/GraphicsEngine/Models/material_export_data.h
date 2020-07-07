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
// Date : 15.01.20
//-----------------------------------------------------------------------------
#pragma once
#include <Utility/color.h>
#include <GraphicsEngine/Images/image_cube.h>
#include <GraphicsEngine/Images/image_2d.h>

namespace poke {
namespace graphics {
/**
 * \brief Special data container ready to be sent to any type of Uniform or Descriptor
 */
class MaterialExportData {
public:
    enum class Type : uint8_t {
        FLOAT = 0,
        INT,
        IMAGE_2D,
        IMAGE_CUBE,
        COLOR
    };

    /**
     * \brief Construct a MaterialExportData that contains a float.
     * \param f 
     */
    explicit MaterialExportData(float f);

    /**
     * \brief Construct a MaterialExportData that contains a integer.
     * \param i 
     */
    explicit MaterialExportData(int i);

    /**
     * \brief Construct a MaterialExportData that contains a ptr to an image 2d.
     * \param image2d 
     */
    explicit MaterialExportData(const Image2d* image2d);

    /**
     * \brief Construct a MaterialExportData that contains a ptr to an cube's image.
     * \param imageCube 
     */
    explicit MaterialExportData(const ImageCube* imageCube);

    /**
     * \brief Construct a MaterialExportData that contains a color.
     * \param color 
     */
    explicit MaterialExportData(Color color);

    const void* operator&() const;

    /**
     * \brief Return type of data contained 
     * \return 
     */
    Type GetType() const { return type_; }

    const Image2d* GetImage2D() const { return image2d_; }

    const ImageCube* GetImageCube() const { return imageCube_; }

private:

    Type type_;

    union {
        float float_ = 0.0f;
        int integer_;
        const Image2d* image2d_;
        const ImageCube* imageCube_;
        Color color_;
    };
};
} //namespace graphics
} //namespace poke
