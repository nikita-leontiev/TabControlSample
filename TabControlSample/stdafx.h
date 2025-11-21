#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "UxTheme.lib")

#include "targetver.h"

#include <inttypes.h>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <uxtheme.h>

#include "resource.h"

#define UNUSED(param) (void)(param);

#define VERIFY(expression) \
	if (!(expression)) { assert(false && #expression); }

#define __W(x) L##x
#define _W(x) __W(x)
#define __WFUNCTION__ _W(__FUNCTION__)

typedef std::vector<HWND> HWNDVector;
typedef HWNDVector::const_iterator HWNDVectorConstIt;
typedef std::map<HWND, HWND> HWNDMap;
typedef HWNDMap::const_iterator HWNDMapConstIt;
