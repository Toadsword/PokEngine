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
// Author : Guillaume Volygesi
// Co-Author :
// Date : 07.04.2020
//----------------------------------------------------------------------------------
#pragma once

#include <Editor/Tools/tool.h>
#include <Editor/editor.h>
#include <Editor/Ecs/editor_ecs_manager.h>
#include <Chunks/chunks_manager.h>
#include <Chunks/chunks.h>

namespace poke {

namespace editor {

class ChunkEditorTool : public Tool {

public:

	explicit ChunkEditorTool(Editor& editor, bool defaultActive = false);

	void OnDrawImgui() override;

	void OnSetActive() override;

	void OnSetInactive() override;

	static std::vector<chunk::ChunkIndex> GetChunksToDisplay();	

private:

	void OnDraw();

	Editor& editor_;
	chunk::ChunksManager& chunkManager_;

	std::vector<ecs::EntityIndex> chunkEntities_;

	std::array<chunk::Chunk, 64> chunkArray_;

	std::vector<std::string> chunkNames_;
	
	const char* kChunkWindow_ = "Chunks";
	const char* kEntitiesWindow_ = "Entities";

	void SetChunkValues(chunk::Chunk chunk);

	void SetChunkNames();

	int selectedIndex_ = 0;
	bool chunkSelected_ = false;

	chunk::ChunkStatus globalStatus_;

	std::vector<chunk::ChunkIndex> chunkIndexes_;
	std::vector<chunk::ChunkIndex> debugVisibleChunkIndexes_;
	std::vector<chunk::ChunkIndex> debugHiddenChunkIndexes_;
	std::vector<ecs::EntityIndex> entityInScene_;
	
	inline static std::vector<chunk::ChunkIndex> chunkToDisplayInGuizmo_ = std::vector<chunk::ChunkIndex>();
	inline static bool isActive = false;

	void SetValuesAtStart();
	int sceneIndex;
};
}//namespace editor
}//namespace poke