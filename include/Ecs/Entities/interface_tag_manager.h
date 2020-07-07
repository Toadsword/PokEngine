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
// Co-Author :
// Date : 24.02.20
//----------------------------------------------------------------------------------
#pragma once
#include <Ecs/ecs_utility.h>
#include <Utility/json_utility.h>

namespace poke {
namespace ecs {
class ITagManager {
public:
    /**
     * \brief Load all tags from a json file.
     * \param tagsJson 
     */
    virtual void SetFromJson(const json& tagsJson) = 0;

    /**
     * \brief Write all tags to a json file.
     * \return 
     */
    virtual json ToJson() = 0;

    /**
     * \brief Return the Tag value from its name.
     * \param tagName 
     * \return 
     */
    virtual EntityTag GetTag(const std::string& tagName) = 0;

    /**
     * \brief Get the maximum value of a tag
     * \return 
     */
    virtual EntityTag GetTagMaxValue() const = 0;

    /**
	 * \brief Get the name of a tag by its name.
	 * \param tag 
	 * \return 
	 */
	virtual const char* GetTagName(EntityTag tag) const = 0;

    /**
	 * \brief Get the name of every tag.
	 * \return 
	 */
	virtual const std::vector<std::string>& GetAllTagNames() const = 0;
};
} //namespace ecs
} //namespace poke
