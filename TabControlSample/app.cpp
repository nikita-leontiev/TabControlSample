#include "stdafx.h"
#include "utils.h"
#include "app.h"

INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CBTProc(int, WPARAM, LPARAM);

HWNDMap CApp::m_focus_map;
HWNDMap CApp::m_reverse_focus_map;

CApp::CApp() :
CDlg(),
m_hInstance(NULL),
m_content(CONTENT_NONE),
m_mode(MODE_NONE),
m_hHook(NULL)
{
}

int CApp::Main(HINSTANCE hInstance)
{
	m_hInstance = hInstance;

	INITCOMMONCONTROLSEX icex = {};
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_STANDARD_CLASSES | ICC_TREEVIEW_CLASSES | ICC_TAB_CLASSES;
	VERIFY(InitCommonControlsEx(&icex));

	//DisableTheme();

	VERIFY(SUCCEEDED(OleInitialize(NULL)));

	Create(hInstance, NULL, IDD_MAIN_DIALOG, DialogProc, reinterpret_cast<LPARAM>(this));

	MSG msg = {};
	BOOL bResult = 0;
	while ((bResult = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (bResult == -1)
			break;
		if (!IsWindow() || !IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

INT_PTR CApp::OnInit(HWND hWnd, HWND hWndFocus, LPARAM lParam)
{
	UNUSED(hWndFocus);
	UNUSED(lParam);

	m_hWnd = hWnd;

	m_create_radio_button.LoadFromDialog(hWnd, IDC_CREATE_RADIOBUTTON);
	m_create_radio_button.Check();
	m_on_dialog_radio_button.LoadFromDialog(hWnd, IDC_ON_DIALOG_RADIOBUTTON);
	m_dialog_radio_button.LoadFromDialog(hWnd, IDC_DIALOG_RADIOBUTTON);
	m_create_dialog_radio_button.LoadFromDialog(hWnd, IDC_CREATE_DIALOG_RADIOBUTTON);

	m_clipping_radio_button.LoadFromDialog(hWnd, IDC_CLIPPING_RADIOBUTTON);
	m_hook_radio_button.LoadFromDialog(hWnd, IDC_HOOK_RADIOBUTTON);
	m_tab_handle_radio_button.LoadFromDialog(hWnd, IDC_TAB_HANDLE_RADIOBUTTON);
	m_none_radio_button.LoadFromDialog(hWnd, IDC_NONE_RADIOBUTTON);
	m_none_radio_button.Check();

	m_main_tree_view.LoadFromDialog(hWnd, IDC_TREEVIEW);
	LoadTreeItems(m_main_tree_view, 0, 1);

	m_tab_control.LoadFromDialog(hWnd, IDC_TAB_CONTROL);
	LoadTabs();

	RECT rect = m_tab_control.GetDisplayArea();
	CTreeView tree_view;
	tree_view.Create(m_hInstance, m_hWnd, IDC_TAB_TREEVIEW1,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	SetupTreeView(tree_view, &rect, 0);

	tree_view.Create(m_hInstance, m_hWnd, IDC_TAB_TREEVIEW2,
		rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	SetupTreeView(tree_view, &rect, 1);

	CreateTabDialog(m_tab_dialog1, false, 0);
	CreateTabDialog(m_tab_dialog2, false, 1);

	RecreateTab();

	return TRUE;
}

void CApp::OnClose(HWND hWnd)
{
	UNUSED(hWnd);
	Destroy();
}

void CApp::OnDestroy(HWND hWnd)
{
	UNUSED(hWnd);
	PostQuitMessage(0);
}

void CApp::OnCommand(HWND hWnd, int nID, HWND hCtrl, UINT nNotify)
{
	UNUSED(hWnd);
	UNUSED(hCtrl);
	UNUSED(nNotify);

	switch (nID)
	{
	case IDC_CREATE_RADIOBUTTON:
	case IDC_ON_DIALOG_RADIOBUTTON:
	case IDC_DIALOG_RADIOBUTTON:
	case IDC_CREATE_DIALOG_RADIOBUTTON:
	case IDC_CLIPPING_RADIOBUTTON:
	case IDC_HOOK_RADIOBUTTON:
	case IDC_TAB_HANDLE_RADIOBUTTON:
	case IDC_NONE_RADIOBUTTON:
		if (Button_GetCheck(hCtrl) == BST_CHECKED)
			RecreateTab();
		break;
	}
}

INT_PTR CApp::OnNotify(HWND hWnd, int nID, const NMHDR *pNMHDR)
{
	UNUSED(hWnd);
	UNUSED(nID);

	switch (pNMHDR -> code)
	{
	case TCN_SELCHANGE:
		if (pNMHDR -> idFrom == IDC_TAB_CONTROL)
		{
			RecreateTab();
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void CApp::RecreateTab()
{
	int nIndex = m_tab_control.GetSelectedTab();

	DisableMode();
	if (m_content == CONTENT_CREATE)
		m_tab_tree_view.Destroy();
	else if (m_content == CONTENT_ON_DIALOG)
		m_tab_tree_view.Hide();
	else if (m_content == CONTENT_DIALOG)
	{
		m_tab_dialog1.Hide();
		m_tab_dialog2.Hide();
	}
	else if (m_content == CONTENT_CREATE_DIALOG)
		m_tab_dialog.Destroy();

	if (m_create_radio_button.IsChecked())
	{
		RECT rect = m_tab_control.GetDisplayArea();
		m_tab_tree_view.Create(m_hInstance, m_hWnd, IDC_TAB_TREEVIEW,
			rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, WS_VISIBLE);
		SetupTreeView(m_tab_tree_view, NULL, nIndex);
		m_content = CONTENT_CREATE;
	}
	else if (m_on_dialog_radio_button.IsChecked())
	{
		m_tab_tree_view.LoadFromDialog(m_hWnd, nIndex?IDC_TAB_TREEVIEW2:IDC_TAB_TREEVIEW1);
		m_tab_tree_view.Show();
		m_content = CONTENT_ON_DIALOG;
	}
	else if (m_dialog_radio_button.IsChecked())
	{
		if (nIndex)
		{
			m_tab_dialog2.Show();
			m_tab_tree_view.LoadFromDialog(m_tab_dialog2.GetHWND(), IDC_TAB_TREEVIEW);
		}
		else
		{
			m_tab_dialog1.Show();
			m_tab_tree_view.LoadFromDialog(m_tab_dialog1.GetHWND(), IDC_TAB_TREEVIEW);
		}
		m_content = CONTENT_DIALOG;
	}
	else if (m_create_dialog_radio_button.IsChecked())
	{
		CreateTabDialog(m_tab_dialog, true, nIndex);
		m_tab_tree_view.LoadFromDialog(m_tab_dialog.GetHWND(), IDC_TAB_TREEVIEW);
		m_content = CONTENT_CREATE_DIALOG;
	}
	EnableMode();
}

void CApp::EnableMode()
{
	if (m_clipping_radio_button.IsChecked())
		SetupClipping();
	else if (m_hook_radio_button.IsChecked())
		SetupHook();
	else if (m_tab_handle_radio_button.IsChecked())
		SetupTabHandle();
}

void CApp::DisableMode()
{
	RemoveClipping();
	RemoveHook();
	RemoveTabHandle();
}

void CApp::SetupClipping()
{
	/* Tab control is located at the top of the z-order, keyboard selection works as expected,
	but tab control overlaps all controls that are located on the tab display area.
	To solve this, tab control is subclassed and control areas are excluded from DC in
	WM_ERASEBKGND handler. Idea: https://www.virtualdub.org/blog2/entry_291.html */
	m_tab_control.SetZOrder(m_main_tree_view.GetHWND());
	m_tab_control.ExcludeSibling(m_tab_tree_view.GetHWND());
	m_tab_control.EnableSubclass();
	m_mode = MODE_CLIPPING;
}

void CApp::RemoveClipping()
{
	if (m_mode == MODE_CLIPPING)
	{
		m_tab_control.DisableSubclass();
		m_tab_control.ClearExcludeSiblings();
		m_mode = MODE_NONE;
	}
}

void CApp::SetupHook()
{
	/* Tab control is located at the bottom of the z-order and doesn't overlap any controls,
	but keyboard selection order is incorrect. To solve this, WH_CBT hook is set to handle HCBT_SETFOCUS
	notify and override default focus processing. */
	assert(!m_hHook);
	m_tab_control.SetZOrder(m_tab_tree_view.GetHWND());
	m_focus_map[m_main_tree_view.GetHWND()] = m_tab_control.GetHWND();
	m_focus_map[m_tab_control.GetHWND()] = m_tab_tree_view.GetHWND();
	m_focus_map[m_tab_tree_view.GetHWND()] = m_create_radio_button.GetHWND();
	m_reverse_focus_map[m_create_radio_button.GetHWND()] = m_tab_tree_view.GetHWND();
	m_reverse_focus_map[m_tab_tree_view.GetHWND()] = m_tab_control.GetHWND();
	m_reverse_focus_map[m_tab_control.GetHWND()] = m_main_tree_view.GetHWND();
	m_hHook = SetWindowsHookEx(WH_CBT, CBTProc, NULL, GetCurrentThreadId());
	assert(m_hHook);
	m_mode = MODE_HOOK;
}

void CApp::RemoveHook()
{
	if (m_mode == MODE_HOOK)
	{
		assert(m_hHook);
		VERIFY(UnhookWindowsHookEx(m_hHook));
		m_hHook = NULL;
		m_focus_map.clear();
		m_reverse_focus_map.clear();
		m_tab_control.SetZOrder(m_main_tree_view.GetHWND());
		m_mode = MODE_NONE;
	}
}

void CApp::SetupTabHandle()
{
	/* Tab control is located at the bottom of the z-order and doesn't overlap any controls,
	but keyboard selection order is incorrect. To solve this, a custom tab key handler introduced
	for controls that break the tab order. */
	m_tab_control.SetZOrder(m_tab_tree_view.GetHWND());
	m_main_tree_view.SetPrevNext(NULL, m_tab_control.GetHWND());
	m_main_tree_view.EnableKeyHandler();
	m_tab_control.SetPrevNext(m_main_tree_view.GetHWND(), m_tab_tree_view.GetHWND());
	m_tab_control.EnableKeyHandler();
	m_tab_tree_view.SetPrevNext(m_tab_control.GetHWND(), m_create_radio_button.GetHWND());
	m_tab_tree_view.EnableKeyHandler();
	m_create_radio_button.SetPrevNext(m_tab_tree_view.GetHWND(), NULL);
	m_create_radio_button.EnableKeyHandler();
	m_mode = MODE_TAB_HANDLER;
}

void CApp::RemoveTabHandle()
{
	if (m_mode == MODE_TAB_HANDLER)
	{
		m_main_tree_view.DisableKeyHandler();
		m_tab_control.DisableKeyHandler();
		m_tab_tree_view.DisableKeyHandler();
		m_create_radio_button.DisableKeyHandler();
		m_tab_control.SetZOrder(m_main_tree_view.GetHWND());
		m_mode = MODE_NONE;
	}
}

void CApp::LoadTabs()
{
	for (size_t i = 0; i < 2; ++i)
	{
		std::wstringstream ss;
		ss << L"Tab_" << i;
		m_tab_control.AddTab(ss.str());
	}
}

void CApp::CreateTabDialog(CDlg & dialog, bool fShow, int nIndex)
{
	dialog.Create(m_hInstance, m_hWnd, IDD_TAB_DIALOG, NULL, NULL);
	dialog.EnableTheme(ETDT_ENABLETAB);
	dialog.Move(m_tab_control.GetDisplayArea());

	CTreeView tree_view;
	tree_view.LoadFromDialog(dialog.GetHWND(), IDC_TAB_TREEVIEW);
	RECT rect = dialog.GetClient();
	SetupTreeView(tree_view, &rect, nIndex);

	/* For the dialog creation mode dialog should be initially hidden:
	1. To prevent dialog drawing before move i.e. flicker. Other way: place move in WM_INITDIALOG.
	2. If dialog is initially visible, treeview will not receive WM_PAINT during last dialog painting cycle. */
	if (fShow)
		dialog.Show();
}

void CApp::SetupTreeView(CTreeView & tree_view, const RECT *pRect, int nIndex)
{
	tree_view.SetFont(GetFont());
	tree_view.RegisterDragDrop(&m_drop_target);
	if (pRect)
		tree_view.Move(*pRect);
	LoadTreeItems(tree_view, nIndex?7:0, 7);
}

void CApp::LoadTreeItems(CTreeView & tree_view, size_t first, size_t count)
{
	count += first;
	for (size_t i = first; i < count; ++i)
	{
		std::wstringstream ss;
		ss << L"Item_" << i;
		HTREEITEM hItem = tree_view.InsertItem(ss.str());
		if (i == count - 1 && hItem)
			tree_view.SelectItem(hItem);
	}
}

HWNDMap & CApp::GetFocusMap()
{
	return m_focus_map;
}

HWNDMap & CApp::GetReverseFocusMap()
{
	return m_reverse_focus_map;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (nMsg == WM_INITDIALOG)
		SetWindowLongPtr(hWnd, GWLP_USERDATA, lParam);

	CApp *pApp = reinterpret_cast<CApp*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (!pApp)
		return FALSE;

	switch (nMsg)
	{
		HANDLE_MSG(hWnd, WM_INITDIALOG, pApp -> OnInit);
		HANDLE_MSG(hWnd, WM_CLOSE, pApp -> OnClose);
		HANDLE_MSG(hWnd, WM_DESTROY, pApp -> OnDestroy);
		HANDLE_MSG(hWnd, WM_COMMAND, pApp -> OnCommand);
		HANDLE_MSG(hWnd, WM_NOTIFY, pApp -> OnNotify);
	}
	return FALSE;
}

LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_SETFOCUS)
	{
		HWND hNew = reinterpret_cast<HWND>(wParam);
		HWND hOld = reinterpret_cast<HWND>(lParam);
		const HWNDMap & focus_map = IsShift()?CApp::GetReverseFocusMap():CApp::GetFocusMap();
		HWNDMapConstIt it = focus_map.find(hOld);
		if (it != focus_map.end() && hNew != it -> second)
		{
			SetFocus(it -> second);
			return 1;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
