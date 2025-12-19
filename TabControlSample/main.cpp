#include "stdafx.h"
#include "app.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	UNUSED(hPrevInstance);
	UNUSED(pCmdLine);
	UNUSED(nCmdShow);

	return CApp().Main(hInstance);
}
