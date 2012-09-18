#include "stdafx.h"
#include "Init.h"
#include "d3dx9.h"
#include "d3d9.h"
#include "d3dx9Mesh.h"

namespace d3d{

HRESULT DX3DInitZ(IDirect3D9 **ppiD3D9, 
                IDirect3DDevice9 **ppiD3DDevice9,
                HWND hWnd, 
				DWORD iW,
				DWORD iH,
				BOOL bFullScreen
				)
{
	int result;
	HRESULT hRes;
	D3DCAPS9 caps; 
	D3DPRESENT_PARAMETERS prm;
	D3DDISPLAYMODE mode;
	DWORD flags; //A combination of one or more flags that control the device create behavior. 

	// создаем Direct3D
	if( NULL == (*ppiD3D9 = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

		
	(*ppiD3D9)->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);    // потом учесть, что мб несколько адаптеров в системе
	if(caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else{
		flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	ZeroMemory(&prm,sizeof(prm));
	prm.BackBufferWidth=iW;
	prm.BackBufferHeight=iH;
	prm.BackBufferCount=1;
	prm.hDeviceWindow=hWnd;
	prm.AutoDepthStencilFormat= D3DFMT_D16; 
	prm.EnableAutoDepthStencil=TRUE; 	//Автоматическое создание и поддержка буфера глубины и трафарета 

	// выбор полноэкранный режим или оконный
	if(!bFullScreen){
		result=MessageBox(hWnd, "Use fullscreen mode?","Screen",MB_YESNO | MB_ICONQUESTION); 
	}
	else{
		result=IDYES;
	}

	(*ppiD3D9)->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mode); //

	if(result==IDYES) // если полноэкранный режим
	{
		// fulscreen
		prm.BackBufferFormat=mode.Format;
		prm.SwapEffect=D3DSWAPEFFECT_FLIP;
		prm.Windowed=FALSE;
		prm.FullScreen_RefreshRateInHz=D3DPRESENT_RATE_DEFAULT;
		prm.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
	} 
	else
	{
		RECT cli_rect,wnd_rect;
		GetWindowRect(hWnd,&wnd_rect);
		GetClientRect(hWnd,&cli_rect);
		iW=iW+(wnd_rect.right-wnd_rect.left)-(cli_rect.right-cli_rect.left);
		iH=iH+(wnd_rect.bottom-wnd_rect.top)-(cli_rect.bottom-cli_rect.bottom);
		MoveWindow(hWnd, wnd_rect.left, wnd_rect.top, iW, iH, TRUE);
		ShowWindow(hWnd,SW_MAXIMIZE);
		prm.BackBufferFormat=mode.Format;
		prm.SwapEffect = D3DSWAPEFFECT_DISCARD;
		prm.Windowed = TRUE;
	}

	if(FAILED(hRes=(*ppiD3D9)->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, prm.BackBufferFormat, prm.BackBufferFormat, prm.Windowed)))
		return hRes;

	// создаем устройство Direct3D
	if( FAILED(hRes=(*ppiD3D9)->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, flags, &prm, ppiD3DDevice9)))
		return hRes;

	// устанавливаем перспективу
	float Aspect = (float)prm.BackBufferWidth / (float)prm.BackBufferHeight;
	D3DXMATRIX matProjection;
	D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI*0.5f, Aspect, 1.0f, 5000.0f);
	(*ppiD3DDevice9)->SetTransform(D3DTS_PROJECTION, &matProjection);

	// выключаем освещение // мб устанавливаем состояние устройства Direct3D
	(*ppiD3DDevice9)->SetRenderState(D3DRS_LIGHTING,NULL);

	return S_OK;
}

}






