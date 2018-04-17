#pragma once


// プリコンパイル済みヘッダー


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
#include <string>
#include <memory>


#include "AppException.h"
#include "App.h"

