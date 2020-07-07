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
// Date : 17.02.20
//-----------------------------------------------------------------------------
#pragma once

#include <GraphicsEngine/Models/model_instance.h>
#include <GraphicsEngine/Models/model.h>

namespace poke {
class Engine;

namespace graphics {
class GraphicEngine;

struct ForwardDrawCmd {
    math::Matrix4 worldMatrix{0};

    XXH64_hash_t materialID;
    XXH64_hash_t meshID;

    DescriptorHandle descriptorSet;
    UniformHandle uniformObject;
};

using ModelInstanceIndex = size_t;
using ModelForwardIndex = int;

/**
 * \brief Drawing command buffer specific to draw models in the deferred pass
 */
class ModelCommandBuffer {
public:
    /**
     * \brief 
     */
    explicit ModelCommandBuffer(Engine& engine);

    ~ModelCommandBuffer();

    void OnEngineInit();

    //TODO(@Nico) Make it const.
    std::vector<std::unique_ptr<ModelInstance>>& GetModelInstances();

    //TODO(@Nico) Make it const.
    std::vector<ForwardDrawCmd>& GetForwardModels() { return forwardDrawingCmd_; }

	ModelInstanceIndex GetModelInstanceIndex(const Material& material, const Mesh& mesh);

    /**
     * \brief Get an index to draw a object with forward rendering.
     * \return 
     */
	ModelInstanceIndex GetForwardIndex();

    /**
     * \brief Free an Index.
     * \details When an entity is destroy and it was drawn in forward rendering, you call this function.
     * \param index 
     */
	void FreeForwardIndex(ModelForwardIndex index);

    /**
     * \brief Function specialized for drawing object that can be GPU Instanced
     * \param worldMatrix 
     * \param instanceIndex 
     */
    void Draw(
		math::Matrix4 worldMatrix,
        ModelInstanceIndex instanceIndex);

    /**
     * \brief Forward drawing
     * \param worldMatrix 
     * \param model
     * \param forwardIndex
     */
    void Draw(
        math::Matrix4 worldMatrix,
        Model model,
		ModelForwardIndex forwardIndex);

	/**
	* \brief Function to draw a unique model
	* \param drawCommand
	*/
	void Draw(const ForwardDrawCmd& drawCommand);

	void Clear();

    void PrepareData();

private:
    void OnUnloadScene();

    static const int kSizePerType = 200;

    //Data for gpu instancing
	std::vector<std::vector<InstancingDrawCmd>> instancesMatrix_;
    std::vector<std::unique_ptr<ModelInstance>> modelInstances_;

    //Data for forward rendering
	uint64_t nextFreeForwardIndex_ = 0;
    std::vector<ForwardDrawCmd> forwardDrawingCmd_;
};
} //namespace graphics
} //namespace poke
