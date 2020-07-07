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
// Date : 04.02.20
//-----------------------------------------------------------------------------
#pragma once

#include <future>
#include <optional_custom.h>

namespace poke {
template<typename T>
class Future {
public:
	Future() noexcept = default;

	Future(std::future<T> &&future) noexcept :
		future_(std::move(future)) {
	}

	bool HasValue() const noexcept {
		return future_.valid() || current_;
	}

	T &Get() noexcept {
		if (future_.valid()) {
			current_ = future_.get();
		}

		return *current_;
	}

	constexpr explicit operator bool() const noexcept { return HasValue(); }
	constexpr operator T &() const noexcept { return *Get(); }

	T &operator*() noexcept { return Get(); }
	T &operator->() noexcept { return Get(); }

	bool operator==(const Future &other) const noexcept {
		return future_ == other.future_ && current_ == other.current_;
	}

	bool operator!=(const Future &other) const noexcept {
		return !operator==(other);
	}
private:
	std::future<T> future_;
	std::experimental::optional<T> current_;
};
}