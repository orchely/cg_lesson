// stdafx.h : 標準のシステム インクルード ファイルのインクルード ファイル、または
// 参照回数が多く、かつあまり変更されない、プロジェクト専用のインクルード ファイル
// を記述します。
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include <crtdbg.h>
#include <d3d11.h>
#include <sstream>
#include <ostream>
#include <iomanip>
#include <array>

#if defined(_DEBUG)
#define _ATL_DEBUG_INTERFACES
#endif
#define _ATL_NO_AUTOMATIC_NAMESPACE
#include <atlbase.h>
