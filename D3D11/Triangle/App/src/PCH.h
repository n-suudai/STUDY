#pragma once


#include <wrl.h>
#include <Eigen/Core>


// プリコンパイル済みヘッダー

#if defined(DEBUG) || defined(_DEBUG)


#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <new>

#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s) _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s) _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _recalloc(p, c, s) _recalloc_dbg(p, c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define _expand(p, s) _expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)


#define ENABLE_MEMORY_LEAK_CHECK() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

#else

#define ENABLE_MEMORY_LEAK_CHECK()

#endif


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>


#include <dxgi.h>
#include <d3d11.h>


#include <cassert>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <fstream>
#include <iterator>


#include "geometry/Color.h"
#include "geometry/VertexFormat.h"
#include "Util.h"
#include "App.h"
#include "AppException.h"

