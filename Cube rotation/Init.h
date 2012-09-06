#include "d3d9.h"
#include "d3dx9.h"

/* Функция инициализации DirectX */
HRESULT DX3DInitZ(IDirect3D9 **ppiD3D9, 
                IDirect3DDevice9 **ppiD3DDevice9,
                HWND hWnd, 
				DWORD iW,
				DWORD iH,
				BOOL bFullScreen
				);