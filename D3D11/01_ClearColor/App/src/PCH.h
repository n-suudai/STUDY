﻿#pragma once


// プリコンパイル済みヘッダー


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>


#include <dxgi.h>
#include <d3d11.h>
#include <wrl.h>


#if _HAS_ITERATOR_DEBUGGING > 0
#   undef _HAS_ITERATOR_DEBUGGING
#   define _HAS_ITERATOR_DEBUGGING  0
#endif

#include <cassert>
#include <string>
#include <sstream>
#include <memory>


#include "Util.h"
#include "App.h"
#include "AppException.h"

