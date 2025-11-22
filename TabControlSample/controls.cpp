#include "stdafx.h"
#include "utils.h"
#include "controls.h"

LRESULT CALLBACK KeyHandleSubclassProc(HWND, UINT, WPARAM, LPARAM);

CWnd::CWnd() :
m_hWnd(NULL),
m_pOldProc(NULL),
m_hPrev(NULL),
m_hNext(NULL)
{
}

HWND CWnd::GetHWND()
{
	return m_hWnd;
}

void CWnd::LoadFromDialog(HWND hDlg, int nID)
{
	m_hWnd = GetDlgItem(hDlg, nID);
	assert(m_hWnd);
}

void CWnd::Destroy()
{
	VERIFY(DestroyWindow(m_hWnd));
}

bool CWnd::IsWindow()
{
	return m_hWnd && ::IsWindow(m_hWnd);
}

void CWnd::SetFont(HFONT hFont)
{
	SendMessage(m_hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);
}

HFONT CWnd::GetFont()
{
	return reinterpret_cast<HFONT>(SendMessage(m_hWnd, WM_GETFONT, 0, 0));
}

WNDPROC CWnd::SetWindowProc(WNDPROC pProc)
{
	return reinterpret_cast<WNDPROC>(SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pProc)));
}

WNDPROC CWnd::GetOldProc()
{
	return m_pOldProc;
}

void CWnd::SetUserData(LONG_PTR lpData)
{
	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, lpData);
}

void CWnd::EnableSubclass(WNDPROC pProc)
{
	assert(!m_pOldProc);
	if (!m_pOldProc)
	{
		SetUserData(reinterpret_cast<LONG_PTR>(this));
		m_pOldProc = SetWindowProc(pProc);
	}
}

void CWnd::DisableSubclass()
{
	if (m_pOldProc)
	{
		SetWindowProc(m_pOldProc);
		m_pOldProc = NULL;
		SetUserData(NULL);
	}
}

void CWnd::Show()
{
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
}

void CWnd::Hide()
{
	ShowWindow(m_hWnd, SW_HIDE);
}

void CWnd::Enable(bool bState)
{
	EnableWindow(m_hWnd, bState);
}

void CWnd::RegisterDragDrop(IDropTarget *pDropTarget)
{
	VERIFY(SUCCEEDED(::RegisterDragDrop(m_hWnd, pDropTarget)));
}

void CWnd::Move(int nX, int nY, int nWidth, int nHeight)
{
	VERIFY(MoveWindow(m_hWnd, nX, nY, nWidth, nHeight, TRUE));
}

void CWnd::Move(const RECT & rect)
{
	Move(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

RECT CWnd::GetRect()
{
	RECT rect = {};
	GetWindowRect(m_hWnd, &rect);
	return rect;
}

RECT CWnd::GetClient()
{
	RECT rect = {};
	GetClientRect(m_hWnd, &rect);
	return rect;
}

void CWnd::SetZOrder(HWND hInsertAfter)
{
	VERIFY(SetWindowPos(m_hWnd, hInsertAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
}

void CWnd::SetPrevNext(HWND hPrev, HWND hNext)
{
	m_hPrev = hPrev;
	m_hNext = hNext;
}

void CWnd::EnableKeyHandler()
{
	EnableSubclass(KeyHandleSubclassProc);
}

void CWnd::DisableKeyHandler()
{
	DisableSubclass();
}

void CWnd::OnKey(HWND hWnd, UINT nVK, BOOL fDown, int nRepeat, UINT nFlags)
{
	if (fDown)
	{
		if (nVK == VK_TAB)
		{
			bool fShift = IsShift();
			HWND hFocus = fShift?m_hPrev:m_hNext;
			WPARAM wParam = hFocus?reinterpret_cast<WPARAM>(hFocus):(fShift?1:0);
			LPARAM lParam = MAKELPARAM(hFocus?TRUE:FALSE, 0);
			HWND hParentWnd = GetParent(hWnd);
			SendMessage(hParentWnd, WM_NEXTDLGCTL, wParam, lParam);
			// Required for correct focus rect update
			SendMessage(hParentWnd, WM_CHANGEUISTATE, MAKEWPARAM(UIS_CLEAR, UISF_HIDEFOCUS), 0);
			return;
		}
		assert(m_pOldProc);
		FORWARD_WM_KEYDOWN(hWnd, nVK, nRepeat, nFlags, m_pOldProc);
	}
}

UINT CWnd::OnGetDlgCode(HWND hWnd, LPMSG lpMsg)
{
	assert(m_pOldProc);
	return FORWARD_WM_GETDLGCODE(hWnd, lpMsg, m_pOldProc) | DLGC_WANTTAB;
}

LRESULT CALLBACK KeyHandleSubclassProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWnd *pWnd = reinterpret_cast<CWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!pWnd)
		return 0;

	switch (nMsg)
	{
		HANDLE_MSG(hWnd, WM_KEYDOWN, pWnd -> OnKey);
		HANDLE_MSG(hWnd, WM_GETDLGCODE, pWnd -> OnGetDlgCode);
	}

	return CallWindowProc(pWnd -> GetOldProc(), hWnd, nMsg, wParam, lParam);
}

INT_PTR CALLBACK DefaultDlgProc(HWND, UINT, WPARAM, LPARAM);

void CDlg::Create(HINSTANCE hInstance, HWND hParentWnd, int nID, DLGPROC lpProc, LPARAM lParam)
{
	if (!lpProc)
		lpProc = DefaultDlgProc;
	m_hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(nID), hParentWnd, lpProc, lParam);
}

void CDlg::EnableTheme(DWORD dwFlags)
{
	VERIFY(SUCCEEDED(EnableThemeDialogTexture(m_hWnd, dwFlags)));
}

bool CDlg::IsDialogMessage(MSG *pMsg)
{
	return ::IsDialogMessage(m_hWnd, pMsg)?true:false;
}

INT_PTR CALLBACK DefaultDlgProc(HWND, UINT, WPARAM, LPARAM)
{
	return FALSE;
}

void CButton::Check()
{
	Button_SetCheck(m_hWnd, BST_CHECKED);
}

bool CButton::IsChecked()
{
	return Button_GetCheck(m_hWnd) == BST_CHECKED;
}

LRESULT CALLBACK TabControlSubclassProc(HWND, UINT, WPARAM, LPARAM);

CTabControl::CTabControl() :
CWnd()
{
}

void CTabControl::AdjustRect(RECT & rect_param)
{
	TabCtrl_AdjustRect(m_hWnd, FALSE, &rect_param);
}

RECT CTabControl::GetDisplayArea()
{
	RECT rect = GetRect();
	MapWindowRect(HWND_DESKTOP, GetParent(m_hWnd), &rect);
	AdjustRect(rect);
	return rect;
}

void CTabControl::AddTab(const std::wstring & text)
{
	TCITEM item = {};
	item.mask = TCIF_TEXT;
	item.pszText = const_cast<wchar_t*>(text.c_str());
	VERIFY(TabCtrl_InsertItem(m_hWnd, GetTabsCount(), &item) != -1);
}

int CTabControl::GetTabsCount()
{
	return TabCtrl_GetItemCount(m_hWnd);
}

int CTabControl::GetSelectedTab()
{
	return TabCtrl_GetCurSel(m_hWnd);
}

void CTabControl::EnableSubclass()
{
	CWnd::EnableSubclass(TabControlSubclassProc);
}

BOOL CTabControl::OnEraseBkgnd(HWND hWnd, HDC hDC)
{
	ExcludeSiblings(hWnd, hDC);
	return FORWARD_WM_ERASEBKGND(hWnd, hDC, m_pOldProc);
}

void CTabControl::ExcludeSiblings(HWND hWnd, HDC hDC) const
{
	for (HWNDVectorConstIt it = m_exclude.begin(); it != m_exclude.end(); ++it)
	{
		RECT rect = {};
		GetWindowRect(*it, &rect);
		MapWindowRect(HWND_DESKTOP, hWnd, &rect);
		ExcludeClipRect(hDC, rect.left, rect.top, rect.right, rect.bottom);
	}
}

void CTabControl::ExcludeSibling(HWND hWnd)
{
	m_exclude.push_back(hWnd);
}

void CTabControl::ClearExcludeSiblings()
{
	m_exclude.clear();
}

LRESULT CALLBACK TabControlSubclassProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CTabControl *pTabControl = reinterpret_cast<CTabControl*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!pTabControl)
		return 0;

	switch (nMsg)
	{
		HANDLE_MSG(hWnd, WM_ERASEBKGND, pTabControl -> OnEraseBkgnd);
	}

	return CallWindowProc(pTabControl -> GetOldProc(), hWnd, nMsg, wParam, lParam);
}

void CTreeView::Create(HINSTANCE hInstance, HWND hParentWnd, int nID, int nX, int nY, int nWidth, int nHeight,
					   DWORD dwStyle)
{
	m_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_TREEVIEW, NULL,
		WS_CHILD | WS_TABSTOP | TVS_SHOWSELALWAYS | dwStyle,
		nX, nY, nWidth, nHeight, hParentWnd,
		reinterpret_cast<HMENU>(nID), hInstance, NULL);
	assert(m_hWnd);
}

HTREEITEM CTreeView::InsertItem(const std::wstring & text)
{
	TVINSERTSTRUCT tvis = {};
	tvis.item.mask = TVIF_TEXT;
	tvis.item.pszText = const_cast<wchar_t*>(text.c_str());
	HTREEITEM hItem = TreeView_InsertItem(m_hWnd, &tvis);
	assert(hItem);
	return hItem;
}

void CTreeView::SelectItem(HTREEITEM hItem)
{
	VERIFY(TreeView_SelectItem(m_hWnd, hItem));
}
