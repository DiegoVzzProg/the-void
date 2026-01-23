#pragma once

#ifdef _DEBUG
#define DEBUG_LOG(x) std::cout << "[THE VOID]: " << x << std::endl
#else
#define DEBUG_LOG(x)
#endif