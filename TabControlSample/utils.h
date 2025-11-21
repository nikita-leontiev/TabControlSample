inline std::wstring GetTimestamp()
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);
	const int nBufferSize = 25;
	wchar_t buffer[nBufferSize];
	VERIFY(GetDateFormat(LOCALE_USER_DEFAULT, NULL, &st, NULL, buffer, nBufferSize));
	std::wstring result = buffer;
	VERIFY(GetTimeFormat(LOCALE_USER_DEFAULT, NULL, &st, NULL, buffer, nBufferSize));
	result += std::wstring(1, L' ') + buffer;
	VERIFY(swprintf_s(buffer, nBufferSize, L"%03u", st.wMilliseconds));
	result += std::wstring(1, L'.') + buffer;
	return result;
}

inline void Log(const wchar_t *msg, ...)
{
	va_list args;
	va_start(args, msg);

	int nLength = _vscwprintf(msg, args);
	assert(nLength > 0);
	if (nLength)
	{
		const size_t buffer_size = nLength + 1;
		wchar_t *buffer = new wchar_t[buffer_size];
		int nRes = vswprintf(buffer, buffer_size, msg, args);
		assert(nRes > 0);
		if (nRes > 0)
		{
			std::wstring message = GetTimestamp() + L" " + buffer + L"\n";
			OutputDebugString(message.c_str());
		}
		delete[] buffer;
	}

	va_end(args);
}

inline bool IsShift()
{
	return (GetKeyState(VK_SHIFT) & 0x8000) == 0x8000;
}

inline void DisableTheme()
{
	SetThemeAppProperties(GetThemeAppProperties() & ~STAP_ALLOW_CONTROLS);
}

inline void PrintZOrder(HWND hParentWnd)
{
	const int nBufferSize = 256;
	wchar_t buffer[nBufferSize];
	HWND hWnd = GetTopWindow(hParentWnd);
	while ((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL)
	{
		GetClassName(hWnd, buffer, nBufferSize);
		Log(L"%s", buffer);
	}
}
