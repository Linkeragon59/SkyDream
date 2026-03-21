#include "Core_Utils.h"

#if WINDOWS_BUILD && DEBUG_BUILD
#include <crtdbg.h>
#endif

void InitMemoryLeaksDetection()
{
#if WINDOWS_BUILD && DEBUG_BUILD
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
#endif
}
