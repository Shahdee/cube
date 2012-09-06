#pragma once
#include "stdafx.h"
#include "SystemStructures.h"
#include "d3dx9.h"
#include "d3d9.h"

enum Region
{
	// если представить себе регионы, как кольца увеличивающегося радиуса, чтобы примерно можно было выделить все воксели содержащиеся внутри кольца заданного радиуса, например для отсечения
	Radius0=0,
	Radius1=1,
	Radius2=2,
	Radius3=3,
	Radius4=4,
	Radius5=5
};


enum Tissue
{
	Fat=0, // жир 
	Bone=1, // кость 
	Muscle=2, // мышца 
	Vessel=3, // сосуд 
	Water=4 // вода 
	// что ещё бывает внутри, спросить у врачей

};

struct Selection
{
	//Voxel arr[100]; //

};

