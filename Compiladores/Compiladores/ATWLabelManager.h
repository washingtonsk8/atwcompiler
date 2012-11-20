#ifndef ATWLABEL
#define ATWLABEL
//-----------------------------------------------------------------------------------------------------
#include "GeneralHashFunctions.h"
#include "ATW_MemoryManager.h"
#include "ATW_LabelTable.h"
#include "EventsManager.h"
#include "Globals.h"
#include "ERRO_DEF.h"
#include <queue>
#include <stdio.h>
#include <stdlib.h>
//-----------------------------------------------------------------------------------------------------
using namespace std;
using namespace MemoryManager;
//-----------------------------------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------------------------------
#define MAX_LABELS 10000
//-----------------------------------------------------------------------------------------------------
namespace LabelController{
	//VARIABLES--------------------------------------------------
	extern EventsManager* _ev;
	extern Address*		  _CurrentPCAdress;
	extern int            _Labels[MAX_LABELS];
	extern int            _RotCount;
	extern queue<int>     _AvailableFreeLabels;
	//VARIABLES--------------------------------------------------

	//METHODS----------------------------------------------------
	extern char*          ATWNovoRot();
	extern void           ATWRemoveLabel(int _Label);
	//METHODS----------------------------------------------------
}
