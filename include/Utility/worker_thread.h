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
// Date : 07.01.2020
//----------------------------------------------------------------------------------
#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace poke
{
class WorkerThread
{
public:
	WorkerThread();

	~WorkerThread();

    /**
	 * \brief Do a task in an asynchronous time
	 * \param task 
	 */
	void DoAsync(const std::function<void()>& task);

    /**
     * \brief Do a task in synchronous time
     * \param task 
     */
    void DoSync(const std::function<void()>& task);

    /**
	 * \brief Wait this worker to finish all its task
	 */
	void Wait();

    /**
	 * \brief Stop the worker
	 */
	void Stop();
private:
	void StartWorker();
	
	std::mutex mutex_;

	volatile bool isRunning_ = true;

	std::queue<std::function<void()>> tasks_;

	std::unique_ptr<std::thread> thread_;
	
	std::condition_variable itemInQueue_;
};
} //namespace poke
