#include "stdafx.h"
#include "drop_target.h"

HRESULT STDMETHODCALLTYPE CDropTarget::QueryInterface(REFIID riid, void **ppvObject)
{
	if (riid == IID_IUnknown || riid == IID_IDropTarget)
	{
		assert(ppvObject);
		*ppvObject = this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE CDropTarget::AddRef()
{
	assert(m_nRef);
	return InterlockedIncrement(&m_nRef);
}

ULONG STDMETHODCALLTYPE CDropTarget::Release()
{
	assert(m_nRef);
	uint32_t nRef = InterlockedDecrement(&m_nRef);
	if (nRef == 0)
		delete this;
	return nRef;
}

HRESULT STDMETHODCALLTYPE CDropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	UNUSED(pDataObj);
	UNUSED(grfKeyState);
	UNUSED(pt);
	UNUSED(pdwEffect);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	UNUSED(grfKeyState);
	UNUSED(pt);
	UNUSED(pdwEffect);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropTarget::DragLeave()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CDropTarget::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	UNUSED(pDataObj);
	UNUSED(grfKeyState);
	UNUSED(pt);
	UNUSED(pdwEffect);
	return S_OK;
}
