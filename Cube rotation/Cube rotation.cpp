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
using namespace d3d;

// Global Variables:
char szWindowClass[]="DirectX9 Project";
char szTitle[]="Cube rotation";

#define KEY_DOWN(vk_code)((GetAsyncKeyState(vk_code) & 0x8000)? 1: 0)

// объект Direct3D
IDirect3D9 *pD3D=NULL;
IDirect3DDevice9 *pD3DDevice=NULL;
D3DMATERIAL9 material;  
D3DLIGHT9 light;


D3DXVECTOR3 position(0.0f, 1.0f, -3.0f);
D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

D3DXMATRIX V;

//int iW=1680;
//int iH=1050;
int iW=1366;
int iH=768;

void DrawScene(HWND hWnd, float delta);
bool Setup();

IDirect3DVertexBuffer9* VB;
IDirect3DIndexBuffer9*  IB;
IDirect3DVertexBuffer9* axisX = 0;
IDirect3DVertexBuffer9* axisY = 0;
IDirect3DVertexBuffer9* axisZ = 0;
IDirect3DVertexBuffer9* pir = 0;

IDirect3DVertexBuffer9* Quad = 0;
IDirect3DTexture9* Tex = 0;

IDirect3DVertexBuffer9* mirrorBuffer = 0;

struct Vertex
{
private:
	 float _x, _y, _z;
	 D3DCOLOR  _c;

public:
     Vertex(){}

     Vertex(float x, float y, float z, D3DCOLOR  c){
          _x = x; _y = y; _z = z;
		  _c =c;
     }
     static const DWORD FVF;
};

struct LightVertex
{
private:
	 float _x, _y, _z;
	 float _nx, _ny, _nz;

public:
     LightVertex(){}

     LightVertex(float x, float y, float z, float nx, float ny, float nz){
          _x = x; _y = y; _z = z;
		  _nx = nx; _ny = ny; _nz = z;  // перепутала местами z координату
     }
     static const DWORD LFVF;
};

struct TexelVertex
{
private:
	 float _x, _y, _z;
	 float _nx, _ny, _nz;
	 float _u, _v;
public:
	TexelVertex(){}

	TexelVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v){
		_x = x; _y = y; _z = z;
		_nx = nx; _ny = ny; _nz = z; 
		_u = u, _v = v;
	}
	static const DWORD TLFVF;
};

const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
const DWORD LightVertex::LFVF = D3DFVF_XYZ | D3DFVF_NORMAL;
const DWORD TexelVertex::TLFVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


void EstimateNormal( D3DXVECTOR3* p0,
					D3DXVECTOR3* p1,
					D3DXVECTOR3* p2,
					D3DXVECTOR3* p3,
					D3DXVECTOR3* out)
{
	D3DXVECTOR3 u = *p1-*p0;
	D3DXVECTOR3 v = *p2-*p0;
	D3DXVec3Cross(out, &u, &v);
	D3DXVec3Normalize(out,out);
}


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
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance,NULL);
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
	if(KEY_DOWN(VK_ESCAPE)){
		// помещает сообщение в очередь ассоциированную с потоком, который создал указаннон окно и возвращается без ожидания обработки сообщения потоком
		PostMessage(hWnd,WM_DESTROY,0,0); 
	}

	if(KEY_DOWN(VK_UP)){
	 position.z+=1;
     //Builds a left-handed, look-at matrix.
     D3DXMatrixLookAtLH(&V, &position, &target, &up);
	// пространство вида
     pD3DDevice->SetTransform(D3DTS_VIEW, &V);
	}

	if(KEY_DOWN(VK_DOWN)){
	 position.z-=1;
     //Builds a left-handed, look-at matrix.
     D3DXMatrixLookAtLH(&V, &position, &target, &up);
	// пространство вида
     pD3DDevice->SetTransform(D3DTS_VIEW, &V);
	}

	/*
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
	pD3DDevice->SetTransform(D3DTS_WORLD, &Ry);
	*/


    // рисование сцены
	// Все вызовы методов рисования должны находится внутри пары вызовов BeginScene и EndScene
	if (SUCCEEDED(pD3DDevice->BeginScene())){	  
		pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, (255,255,255), 1.0f, 0);

			pD3DDevice->SetStreamSource(0, Quad, 0, sizeof(TexelVertex));
			pD3DDevice->SetFVF(TexelVertex::TLFVF);
			pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);


			/*
			pD3DDevice->SetStreamSource(0, VB, 0, sizeof(Vertex)); // первый параметр число потоков, потом ук. на буфер вершин
			pD3DDevice->SetIndices(IB);
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0, 0, 8, 0, 12);
			*/
			//pD3DDevice->SetStreamSource(0, pir, 0, sizeof(LightVertex));
			//pD3DDevice->SetFVF(LightVertex::LFVF);
			//pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,4);
			/*
			pD3DDevice->SetStreamSource(0,axisY,0,sizeof(Vertex));
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,1); 

			pD3DDevice->SetStreamSource(0,axisX,0,sizeof(Vertex));
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,1); 

			pD3DDevice->SetStreamSource(0,axisZ,0,sizeof(Vertex));
			pD3DDevice->SetFVF(Vertex::FVF);
			pD3DDevice->DrawPrimitive(D3DPT_LINELIST,0,1); 
			*/
		pD3DDevice->EndScene();
	}

	/* Presents the contents of the next buffer in the sequence of back buffers owned by the device.
	Present will fail, returning D3DERR_INVALIDCALL, if called between BeginScene and EndScene pairs
	unless the render target is not the current render target 
	(such as the back buffer you get from creating an additional swap chain).
	This is a new behavior for Direct3D 9.*/ 
	pD3DDevice->Present(NULL, NULL, NULL, NULL);  
}

/*Инициализация буферов и объектов*/
bool Setup()
{
	//pD3DDevice->SetRenderState(D3DRS_LIGHTING, true); // necessary to use the light in scene  // don't need it for texture example

	/*
	 // Создание буфера вершин
	pD3DDevice->CreateVertexBuffer(
					12*sizeof(LightVertex),
					D3DUSAGE_WRITEONLY,
					LightVertex::LFVF,
					D3DPOOL_MANAGED,
					&pir,
					0);

   	 LightVertex* lightVer;
	 pir->Lock(0, 0, (void**)&lightVer, 0);
	 // front face
	 lightVer[0] = LightVertex(-1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
     lightVer[1] = LightVertex( 0.0f, 1.0f,  0.0f, 0.0f, 0.707f, -0.707f);
     lightVer[2] = LightVertex( 1.0f, 0.0f, -1.0f, 0.0f, 0.707f, -0.707f);
     // левая грань
     lightVer[3] = LightVertex(-1.0f, 0.0f,  1.0f, -0.707f, 0.707f, 0.0f);
     lightVer[4] = LightVertex( 0.0f, 1.0f,  0.0f, -0.707f, 0.707f, 0.0f);
     lightVer[5] = LightVertex(-1.0f, 0.0f, -1.0f, -0.707f, 0.707f, 0.0f);
     // правая грань
     lightVer[6] = LightVertex( 1.0f, 0.0f, -1.0f, 0.707f, 0.707f, 0.0f);
     lightVer[7] = LightVertex( 0.0f, 1.0f,  0.0f, 0.707f, 0.707f, 0.0f);
     lightVer[8] = LightVertex( 1.0f, 0.0f,  1.0f, 0.707f, 0.707f, 0.0f);
     // задняя грань
     lightVer[9]  = LightVertex( 1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
     lightVer[10] = LightVertex( 0.0f, 1.0f, 0.0f, 0.0f, 0.707f, 0.707f);
     lightVer[11] = LightVertex(-1.0f, 0.0f, 1.0f, 0.0f, 0.707f, 0.707f);
	 pir->Unlock();
	 */

	 pD3DDevice->CreateVertexBuffer(
					6*sizeof(TexelVertex),
					D3DUSAGE_WRITEONLY,
					TexelVertex::TLFVF,
					D3DPOOL_MANAGED,
					&Quad,
					0);
	 TexelVertex* v;
	 Quad->Lock(0,0, (void**)&v, 0);

	 v[0] = TexelVertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
     v[1] = TexelVertex(-1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
     v[2] = TexelVertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

     v[3] = TexelVertex(-1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
     v[4] = TexelVertex( 1.0f,  1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
     v[5] = TexelVertex( 1.0f, -1.0f, 1.25f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	 Quad->Unlock();

	 D3DXCreateTextureFromFile(
					pD3DDevice,
					"dx5_logo.bmp",
					&Tex);

	 pD3DDevice->SetTexture(0, Tex);
	 
	 pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	 pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	 pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	 /*
	 ZeroMemory(&material, sizeof(material));
	 material.Ambient  = d3d::WHITE;
	 material.Diffuse  = d3d::WHITE;
	 material.Specular = d3d::WHITE;
	 material.Emissive = d3d::BLACK;
	 material.Power    = 5.0f;
	 pD3DDevice->SetMaterial(&material);

	 ZeroMemory(&light, sizeof(light));
	 light.Type      = D3DLIGHT_DIRECTIONAL;
	 light.Diffuse   = d3d::WHITE;
     light.Specular  = d3d::WHITE * 0.3f;
     light.Ambient   = d3d::WHITE * 0.6f;
     light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
     pD3DDevice->SetLight(0, &light);
     pD3DDevice->LightEnable(0, true);
	 */


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
	// pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);  // добавление этой строки, улучшило визуализацию света
	//pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);

	 pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);

     return true;
}


void MirrorSetup()
{
	pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, true);
	pD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS); // проверка трафарета всегда завершается успешна, установка операции сравнения
	pD3DDevice->SetRenderState(D3DRS_STENCILREF, 0x1);
	pD3DDevice->SetRenderState(D3DRS_STENCILMASK,      0xffffffff);
	pD3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	pD3DDevice->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
	pD3DDevice->SetRenderState(D3DRS_STENCILPASS , D3DSTENCILOP_REPLACE);

	pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, false);
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);

	// -- dont understand this code part
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	pD3DDevice->SetStreamSource(0, mirrorBuffer, 0, sizeof(TexelVertex));
	pD3DDevice->SetFVF(TexelVertex::TLFVF);
	pD3DDevice->SetMaterial();// material
	pD3DDevice->SetTexture();//texture
	D3DXMATRIX *I;
	D3DXMatrixIdentity(I);
	pD3DDevice->SetTransform(D3DTS_WORLD, I);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
	pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);

	pD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pD3DDevice->SetRenderState();








}


