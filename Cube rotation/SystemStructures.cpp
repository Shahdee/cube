#pragma once
#include "stdafx.h"
#include "SystemStructures.h"
#include "d3dx9.h"
#include "d3d9.h"

enum Region
{
	// ���� ����������� ���� �������, ��� ������ ���������������� �������, ����� �������� ����� ���� �������� ��� ������� ������������ ������ ������ ��������� �������, �������� ��� ���������
	Radius0=0,
	Radius1=1,
	Radius2=2,
	Radius3=3,
	Radius4=4,
	Radius5=5
};


enum Tissue
{
	Fat=0, // ��� 
	Bone=1, // ����� 
	Muscle=2, // ����� 
	Vessel=3, // ����� 
	Water=4 // ���� 
	// ��� ��� ������ ������, �������� � ������

};

struct Selection
{
	//Voxel arr[100]; //

};

