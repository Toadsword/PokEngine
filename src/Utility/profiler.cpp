#include <Utility/profiler.h>

#ifndef NO_PROFILING
#include <string>
#include <Remotery.h>

namespace poke
{

Profiler & Profiler::Get()
{
	static Profiler instance;
	return instance;
}

Profiler::Profiler()
{
	rmt = nullptr;
	rmt_CreateGlobalInstance(&rmt);
}

Profiler::~Profiler()
{
	rmt_DestroyGlobalInstance(rmt);
}
} // namespace poke
#endif
