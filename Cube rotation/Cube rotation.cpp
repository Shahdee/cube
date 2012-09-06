#include "stdafx.h"
#include "Cube rotation.h"
#include "Init.h"
#include "d3dx9.h"
#include "d3d9.h"
#include "Mmsystem.h"
#include <cstdio>
#include "windows.h"
#include "windowsx.h"
#include <iostream>
#include <fstream>
#include "SystemStructures.h"



using namespace std;


// Global Variables:
char szWindowClass[]="DirectX9 Project";
char szTitle[]="Cube rotation";

#define KEY_DOWN(vk_code)((GetAsyncKeyState(vk_code) & 0x8000)? 1: 0)

// объект Direct3D
IDirect3D9 *pD3D=NULL;
IDirect3DDevice9 *pD3DDevice=NULL;

D3DXVECTOR3 position(0.0f, 0.0f, 50.0f);
D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
D3DXMATRIX V;

int iW=1680;
int iH=1050;

void DrawScene(HWND hWnd, float delta);
bool Setup();

IDirect3DVertexBuffer9* VB;
IDirect3DIndexBuffer9*  IB;
IDirect3DVertexBuffer9* axisX = 0;
IDirect3DVertexBuffer9* axisY = 0;
IDirect3DVertexBuffer9* axisZ = 0;

struct Vertex
{
     Vertex(){}
     Vertex(float x, float y, float z)
     {
          _x = x; _y = y; _z = z;
     }
     float _x, _y, _z;
     static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ;




#define MAX_LOADSTRING 100

int WINAPI			WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
bool Init(HWND hWnd);
void GraphEngine(HWND hWnd);

int WINAPI WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	
 	WNDCLASS  winclass;
	MSG msg;
	HWND hWnd;
	HDC hdc;   // device context
	PAINTSTRUCT ps; // paint structure
	
	winclass.style = CS_DBLCLKS |CS_OWNDC |
		             CS_HREDRAW | CS_VREDRAW;
	winclass.style = CS_DBLCLKS |CS_OWNDC |
		             CS_HREDRAW | CS_VREDRAW;

	winclass.lpfnWndProc = WndProc;
	winclass.cbClsExtra =0;
	winclass.cbWndExtra =0;
	winclass.hInstance=hInstance;
	winclass.hIcon= LoadIcon(NULL,IDI_APPLICATION);
	winclass.hCursor= LoadCursor(NULL,IDC_ARROW);
	winclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName= NULL;
	winclass.lpszClassName= szWindowClass;

	if(!RegisterClass(&winclass))
		return false;
	
	// создаем  окно
	hWnd=CreateWindow(szWindowClass,szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_DEFAULT,
		CW_USEDEFAULT, iW, iH, NULL, NULL, hInstance,NULL);
	if(!hWnd)
		return false;

	// отображаем окно
	ShowWindow(hWnd,SW_NORMAL);
	UpdateWindow(hWnd);
	
	//initialization
	if(Init(hWnd)){
		static float lastTime = (float)timeGetTime(); 
		while(true)
		{
			if(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE)) // рассылает входящие посланные сообщения, проверяет очередь сообщений потока на наличие размещенного сообщения и извлекает сообщение
			{
				TranslateMessage(&msg); // переводит сообщение из формата виртуальной клавиатуры в символьное. Дальше оно размещается в очереди сообщений вызывающего потока и оно будет обработано в след раз, когда поток вызывает методы GetMessage или PeekMessage
				DispatchMessage(&msg); //оповещает систему, что надо вызвать оконную процедуру
				if(msg.message==WM_QUIT)
					break;
			}
			float currTime  = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001;

			DrawScene(hWnd,timeDelta);

			lastTime  = currTime;
		}
		return (msg.wParam);// mine
	}

  CoUninitialize();

  return 0;
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



bool Init(HWND hWnd)
{
	if(DX3DInitZ(&pD3D, &pD3DDevice, hWnd, iW,iH,FALSE)!=S_OK)
	{
		MessageBox(hWnd,"DirectX init error","Error",MB_OK);
		return false;
	}
	Setup(); // создание буферов, инициализация объектов
	return true;
}


void DrawScene(HWND hWnd,float delta)
{
	if(KEY_DOWN(VK_ESCAPE))
	{
		// помещает сообщение в очередь ассоциированную с потоком, который создал указаннон окно и возвращается без ожидания обработки сообщения потоком
		PostMessage(hWnd,WM_DESTROY,0,0); 
		
	}
	if(KEY_DOWN(VK_UP))
	{
	 position.z+=10;
	
     //Builds a left-handed, look-at matrix.
     D3DXMatrixLookAtLH(&V, &position, &target, &up);

	// пространство вида
     pD3DDevice->SetTransform(D3DTS_VIEW, &V);

	}

	if(KEY_DOWN(VK_DOWN))
	{
	 position.z-=10;
     //Builds a left-handed, look-at matrix.
     D3DXMatrixLookAtLH(&V, &position, &target, &up);

	// пространство вида
     pD3DDevice->SetTransform(D3DTS_VIEW, &V);
	}

		  D3DXMATRIX Rx, Ry;

          // поворот на 45 градусов вокруг оси X
          D3DXMatrixRotationX(&Rx, 3.14f / 4.0f);

          // увеличение угла поворота вокруг оси Y в каждом кадре
          static float y = 0.0f;
          D3DXMatrixRotationY(&Ry, y);
          y += delta;

          // сброс угла поворота, если он достиг 2*PI
          if( y >= 6.28f )
               y = 0.0f;

          // комбинация поворотов
          D3DXMATRIX p = Rx * Ry;

          pD3DDevice->SetTransform(D3DTS_WORLD, &p);

          //
          // рисование сцены:

		// Все вызовы методов рисования должны находится внутри пары вызовов BeginScene и EndScene
		if (SUCCEEDED(pD3DDevice->BeginScene())) 
		{	  
			pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB(255,0,0), 1.0f, 0);
			pD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex));
			pD3DDevice->SetIndices(IB);
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0, 0, 8, 0, 12); 
		

			pD3DDevice->SetStreamSource(0,axisY,0,sizeof(Vertex));
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,1); 

			pD3DDevice->SetStreamSource(0,axisX,0,sizeof(Vertex));
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,1); 


			pD3DDevice->SetStreamSource(0,axisZ,0,sizeof(Vertex));
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,1); 

			pD3DDevice->EndScene();
		}

		// Presents the contents of the next buffer in the sequence of back buffers owned by the device.
		// Present will fail, returning D3DERR_INVALIDCALL, if called between BeginScene and EndScene pairs unless the render target is not the current render target (such as the back buffer you get from creating an additional swap chain). This is a new behavior for Direct3D 9. 
		pD3DDevice->Present(NULL, NULL, NULL, NULL);  
}

/*Инициализация буферов и объектов*/
bool Setup()
{


	 // Создание буфера вершин и буфера индексов
     pD3DDevice->CreateVertexBuffer(
                   8 * sizeof(Vertex),
                   D3DUSAGE_WRITEONLY,
                   Vertex::FVF,
                   D3DPOOL_MANAGED,
                   &VB,
                   0);

     pD3DDevice->CreateIndexBuffer(
                   36 * sizeof(WORD),
                   D3DUSAGE_WRITEONLY,  // это значит что я не могу читать из этого буфера. только запись данных
                   D3DFMT_INDEX16,
                   D3DPOOL_MANAGED,
                   &IB,
                   0);

	 pD3DDevice->CreateVertexBuffer(
                   8 * sizeof(Vertex),
                   D3DUSAGE_WRITEONLY,
                   Vertex::FVF,
                   D3DPOOL_MANAGED,
                   &axisX,
                   0);

	 pD3DDevice->CreateVertexBuffer(
                   8 * sizeof(Vertex),
                   D3DUSAGE_WRITEONLY,
                   Vertex::FVF,
                   D3DPOOL_MANAGED,
                   &axisY,
                   0);

	pD3DDevice->CreateVertexBuffer(
                   8 * sizeof(Vertex),
                   D3DUSAGE_WRITEONLY, 
                   Vertex::FVF,
                   D3DPOOL_MANAGED,
                   &axisZ,
                   0);
	 
	 Vertex* drcY;
	 axisY->Lock(0,0,(void**)&drcY,0);  // здесь произошло чтение по нулевому адресу, разобраться чуть позже
	 drcY[0] = Vertex(0.0f,  0.0f, 0.0f);
	 drcY[1] = Vertex(0.0f,  10.0f, 0.0f);
	 axisY->Unlock();

	 Vertex* drcX;
	 axisX->Lock(0,0,(void**)&drcX,0);
	 drcX[0] = Vertex(0.0f,  0.0f, 0.0f);
	 drcX[1] = Vertex(10.0f, 0.0f, 0.0f);
	 axisX->Unlock();

	 Vertex* drcZ;
	 axisZ->Lock(0,0,(void**)&drcZ,0);
	 drcZ[0] = Vertex(0.0f,  0.0f, 0.0f);
	 drcZ[1] = Vertex(0.0f,  0.0f, 10.0f);
	 axisZ->Unlock();

     // Заполнение буферов данными куба
     Vertex* vertices;
     VB->Lock(0, 0, (void**)&vertices, 0);

     // Вершины единичного куба
	 
     vertices[0] = Vertex(-1.0f, -1.0f, -1.0f);
     vertices[1] = Vertex(-1.0f,  1.0f, -1.0f);
     vertices[2] = Vertex( 1.0f,  1.0f, -1.0f);
     vertices[3] = Vertex( 1.0f, -1.0f, -1.0f);
     vertices[4] = Vertex(-1.0f, -1.0f,  1.0f);
     vertices[5] = Vertex(-1.0f,  1.0f,  1.0f);
     vertices[6] = Vertex( 1.0f,  1.0f,  1.0f);
     vertices[7] = Vertex( 1.0f, -1.0f,  1.0f);

     VB->Unlock();

     // Описание образующих куб треугольников
     WORD* indices = 0;
     IB->Lock(0, 0, (void**)&indices, 0);

     // передняя грань
     indices[0]  = 0; indices[1]  = 1; indices[2]  = 2;
     indices[3]  = 0; indices[4]  = 2; indices[5]  = 3;

     // задняя грань
     indices[6]  = 4; indices[7]  = 6; indices[8]  = 5;
     indices[9]  = 4; indices[10] = 7; indices[11] = 6;

     // левая грань
     indices[12] = 4; indices[13] = 5; indices[14] = 1;
     indices[15] = 4; indices[16] = 1; indices[17] = 0;

     // правая грань
     indices[18] = 3; indices[19] = 2; indices[20] = 6;
     indices[21] = 3; indices[22] = 6; indices[23] = 7;

     // верх
     indices[24] = 1; indices[25] = 5; indices[26] = 6;
     indices[27] = 1; indices[28] = 6; indices[29] = 2;

     // низ
     indices[30] = 4; indices[31] = 0; indices[32] = 3;
     indices[33] = 4; indices[34] = 3; indices[35] = 7;

     IB->Unlock();

     // размещение и ориентация камеры

	 //Builds a left-handed, look-at matrix.
     D3DXMatrixLookAtLH(&V, &position, &target, &up);

	// пространство вида
     pD3DDevice->SetTransform(D3DTS_VIEW, &V);

     // установка матрицы проекции
     D3DXMATRIX proj;

	 //Builds a left-handed perspective projection matrix based on a field of view  . znear and zfar
     D3DXMatrixPerspectiveFovLH(
             &proj,
             D3DX_PI * 0.5f, // 90 градусов
             (float)iW / (float)iH,
             1.0f,
             1000.0f);
	 // пространство проекции
     pD3DDevice->SetTransform(D3DTS_PROJECTION, &proj);

	 D3DVIEWPORT9 vp = { 0, 0, iW, iH, 0, 1.0f }; // viewport работает, видимо когда я его явно не указываю, он в полноэкранном режиме используется по default с параметрами размеров окна
	 pD3DDevice->SetViewport(&vp);

     // установка режима визуализации
	 pD3DDevice->SetRenderState(D3DRS_FILLMODE,  D3DFILL_WIREFRAME);
     return true;
}


