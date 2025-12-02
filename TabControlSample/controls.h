class CWnd
{
protected:
	HWND m_hWnd;
	WNDPROC m_pOldProc;
	HWND m_hPrev;
	HWND m_hNext;
public:
	CWnd();

	HWND GetHWND();
	void LoadFromDialog(HWND hDlg, int nID);
	void Destroy();
	bool IsWindow();
	void SetFont(HFONT hFont);
	HFONT GetFont();
	WNDPROC SetWindowProc(WNDPROC pProc);
	WNDPROC GetOldProc();
	void SetUserData(LONG_PTR lpData);
	void EnableSubclass(WNDPROC pProc);
	void DisableSubclass();
	void Show();
	void Hide();
	void Enable(bool bState);
	void RegisterDragDrop(IDropTarget *pDropTarget);
	void Move(int nX, int nY, int nWidth, int nHeight);
	void Move(const RECT & rect);
	RECT GetRect();
	RECT GetClient();
	void SetZOrder(HWND hInsertAfter);
	void SetPrevNext(HWND hPrev, HWND hNext);
	void EnableKeyHandler();
	void DisableKeyHandler();
	void OnKey(HWND hWnd, UINT nVK, BOOL fDown, int nRepeat, UINT nFlags);
	UINT OnGetDlgCode(HWND hWnd, LPMSG lpMsg);
	static LRESULT CALLBACK KeyHandleSubclassProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
};

class CDlg : public CWnd
{
public:
	void Create(HINSTANCE hInstance, HWND hParentWnd, int nID, DLGPROC lpProc, LPARAM lParam);
	void EnableTheme(DWORD dwFlags);
	bool IsDialogMessage(MSG *pMsg);
	static INT_PTR CALLBACK DefaultDlgProc(HWND, UINT, WPARAM, LPARAM);
};

class CButton : public CWnd
{
public:
	void Check();
	bool IsChecked();
};

class CTabControl : public CWnd
{
private:
	HWNDVector m_exclude;
public:
	CTabControl();

	void AdjustRect(RECT & rect_param);
	RECT GetDisplayArea();
	void AddTab(const std::wstring & text);
	int GetTabsCount();
	int GetSelectedTab();
	void EnableSubclass();
	BOOL OnEraseBkgnd(HWND hWnd, HDC hDC);
	void ExcludeSiblings(HWND hWnd, HDC hDC) const;
	void ExcludeSibling(HWND hWnd);
	void ClearExcludeSiblings();
	static LRESULT CALLBACK TabControlSubclassProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
};

class CTreeView : public CWnd
{
public:
	void Create(HINSTANCE hInstance, HWND hParentWnd, int nID, int nX, int nY, int nWidth, int nHeight,
		DWORD dwStyle = 0);
	HTREEITEM InsertItem(const std::wstring & text);
	void SelectItem(HTREEITEM hItem);
};
