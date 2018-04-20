#pragma once


// プリコンパイル済みヘッダー


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>


#ifndef GLEW_STATIC
#   define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GL/wglew.h>


#if _HAS_ITERATOR_DEBUGGING > 0
#   undef _HAS_ITERATOR_DEBUGGING
#   define _HAS_ITERATOR_DEBUGGING  0
#endif

#include <cassert>
#include <string>
#include <sstream>
#include <memory>


#include "AppException.h"
#include "App.h"
#include "geometry/VertexFormat.h"
#include "shader/SimpleShader.h"



