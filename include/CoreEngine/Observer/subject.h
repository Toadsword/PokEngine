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
// Date : 28.02.20
//-----------------------------------------------------------------------------
#pragma once

#include <functional>
#include <vector>

namespace poke::observer {
/**
 * \brief Subject to observe that contain an array of observers that are called when the function Notify is called.
 */
template <typename... Args>
class Subject  {
public:
    /**
     * \brief
     */
    explicit Subject()
    {
        observers_.reserve(kDefaultSize);
    }

    /**
     * \brief Add an observer.
     * \param observerCallback is a lambda function that will be call when Notify is called.
     */
    void AddObserver(const std::function<void(Args...)>& observerCallback)
    {
        if (observers_.size() == observers_.capacity()) {
            observers_.reserve(observers_.size() * kSizeModifier);
        }

        observers_.emplace_back(observerCallback);
    }

	template<typename T, typename... U>
	size_t GetAddress(std::function<T(U...)> f) {
		typedef T(fnType)(U...);
		fnType ** fnPointer = f.template target<fnType*>();
		return (size_t)*fnPointer;
	}

    /**
     * \brief Remove an observer.
     * \param observerCallback it's the callback that need to be removed.
     */
    void RemoveObserver(const std::function<void(Args...)>& observerCallback)
    {
		for(size_t i = 0; i < observers_.size(); i++) {
		    if(GetAddress(observers_[i]) == GetAddress(observerCallback)) {
				observers_.erase(observers_.begin() + i);
                return;
		    }
		}
    }

	/**
	 * \brief Notify all observers. If the subject is unique-call, then all observers will be erased.
	 */
    void Notify(Args ... args) const
    {
        for (auto& observer : observers_) { observer(std::forward<Args>(args)...); }
    }

    /**
     * \brief Clear all currents observer and resize to default size
     */
    void Clear()
    {
        observers_.clear();
        observers_.reserve(kDefaultSize);
    }

private:

    std::vector<std::function<void(Args...)>> observers_;

    static const size_t kDefaultSize = 10;
    static const size_t kSizeModifier = 2;
};
} //namespace poke::observer
