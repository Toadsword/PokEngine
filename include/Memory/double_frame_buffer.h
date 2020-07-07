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
// Date : 13.11.19
//----------------------------------------------------------------------------------
#pragma once

#include <vector>

namespace poke
{
namespace memory
{

template<typename T>
class DoubleFrameBuffer
{
public:
	DoubleFrameBuffer(const int maxSize = 0)
	{
		current_.reserve(maxSize);
		next_.reserve(maxSize);
	}
	
	~DoubleFrameBuffer() = default;

	template<typename... Args>
	void emplace_back(Args&&... args)
	{
		current_.emplace_back(std::forward<Args>(args)...);
	}

	T& operator[](const std::size_t idx) { return current_.at(idx); }
	const T& operator[](const std::size_t idx) const { return current_.at(idx); }

	std::vector<T>& Read() const
	{
		return next_;
	}

	std::vector<T>& Read()
	{
		return next_;
	}

	void Swap()
	{
		std::swap(current_, next_);
	}
private:
	std::vector<T> current_;
	std::vector<T> next_;
};
} //namespace memory
using namespace memory;
} //namespace poke