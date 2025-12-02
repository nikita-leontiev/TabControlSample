#include "controls.h"
#include "drop_target.h"

#define IDC_TAB_TREEVIEW1 1102
#define IDC_TAB_TREEVIEW2 1103

typedef enum
{
	CONTENT_NONE,
	CONTENT_CREATE,
	CONTENT_ON_DIALOG,
	CONTENT_DIALOG,
	CONTENT_CREATE_DIALOG
} TContent;

typedef enum
{
	MODE_NONE,
	MODE_CLIPPING,
	MODE_HOOK,
	MODE_TAB_HANDLER
} TMode;

class CApp : public CDlg
{
private:
	HINSTANCE m_hInstance;
	CButton m_create_radio_button;
	CButton m_on_dialog_radio_button;
	CButton m_dialog_radio_button;
	CButton m_create_dialog_radio_button;
	CButton m_clipping_radio_button;
	CButton m_hook_radio_button;
	CButton m_tab_handle_radio_button;
	CButton m_none_radio_button;
	CTreeView m_main_tree_view;
	CTabControl m_tab_control;
	CTreeView m_tab_tree_view;
	CDlg m_tab_dialog1;
	CDlg m_tab_dialog2;
	CDlg m_tab_dialog;
	CDropTarget m_drop_target;
	TContent m_content;
	TMode m_mode;
	HHOOK m_hHook;
	static HWNDMap m_focus_map;
	static HWNDMap m_reverse_focus_map;
public:
	CApp();

	int Main(HINSTANCE hInstance);
	INT_PTR OnInit(HWND hWnd, HWND hWndFocus, LPARAM lParam);
	void OnClose(HWND hWnd);
	static void OnDestroy(HWND hWnd);
	void OnCommand(HWND hWnd, int nID, HWND hCtrl, UINT nNotify);
	INT_PTR OnNotify(HWND hWnd, int nID, const NMHDR *pNMHDR);
	void RecreateTab();
	void EnableMode();
	void DisableMode();
	void SetupClipping();
	void RemoveClipping();
	void SetupHook();
	void RemoveHook();
	void SetupTabHandle();
	void RemoveTabHandle();
	void LoadTabs();
	void CreateTabDialog(CDlg & dialog, bool fShow, int nIndex);
	void SetupTreeView(CTreeView & tree_view, const RECT *pRect, int nIndex);
	static void LoadTreeItems(CTreeView & tree_view, size_t first, size_t count);
	static HWNDMap & GetFocusMap();
	static HWNDMap & GetReverseFocusMap();
	static INT_PTR CALLBACK DialogProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK CBTProc(int nCode, WPARAM wParam, LPARAM lParam);
};
