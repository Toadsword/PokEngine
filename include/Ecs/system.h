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
// Author : Robin Alves
// Co-Author : Nicolas Schneider
// Date : 19.11.19
//----------------------------------------------------------------------------------

#pragma once

#include <CoreEngine/module.h>
#include <Ecs/ecs_utility.h>
#include <Ecs/interface_ecs_manager.h>

namespace poke{
namespace ecs{
class CoreEcsManager;

class System : public Module{
public:
	System(Engine& engine);
    
	virtual ~System() = default;
protected:
    //callbacks functions
	virtual void OnEntityInit(EntityIndex entityIndex)
	{
		entityIndex;
	}
	virtual void OnEntityDestroy(EntityIndex entityIndex)
	{
		entityIndex;
	}
	virtual void OnCollisionEnter(EntityIndex entityIndex)
	{
		entityIndex;
	}
	virtual void OnCollisionExit(EntityIndex entityIndex)
	{
		entityIndex;
	}
	virtual void OnEntitySetActive(EntityIndex entityIndex)
	{
		entityIndex;
	}
	virtual void OnEntitySetInactive(EntityIndex entityIndex)
	{
		entityIndex;
	}
	virtual void OnEntityAddComponent(EntityIndex entityIndex, ComponentMask component)
	{
		entityIndex;
		component;
	}
	virtual void OnEntityRemoveComponent(EntityIndex entityIndex, ComponentMask component)
	{
		entityIndex;
		component;
	}

    //observers register
    void ObserveEntityInit();
	void ObserveEntityDestroy();
	void ObserveEntitySetActive();
	void ObserveEntitySetInactive();
	void ObserveEntityAddComponent();
	void ObserveEntityRemoveComponent();

	IEcsManager& ecsManager_;
};
}//namespace ecs
}//namespace poke