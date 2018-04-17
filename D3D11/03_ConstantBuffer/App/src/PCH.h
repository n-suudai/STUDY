#pragma once


#include <wrl.h>


#define EIGEN_NO_DEBUG 1 // コード内のDAssertを無効化
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>


// プリコンパイル済みヘッダー

#if defined(DEBUG) || defined(_DEBUG)

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#define ENABLE_MEMORY_LEAK_CHECK() _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF)

#else

#define ENABLE_MEMORY_LEAK_CHECK()

#endif


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>


#include <dxgi.h>
#include <d3d11.h>


#if _HAS_ITERATOR_DEBUGGING > 0
#   undef _HAS_ITERATOR_DEBUGGING
#   define _HAS_ITERATOR_DEBUGGING  0
#endif

#include <cassert>
#include <cmath>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <fstream>
#include <iterator>


#include "geometry/Color.h"
#include "geometry/VertexFormat.h"
#include "AppException.h"
#include "Util.h"
#include "App.h"

