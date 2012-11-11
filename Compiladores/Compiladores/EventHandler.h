#ifndef	EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <list>
#include "EventsManager.h"
#include "ProgramComponent.h"
#include "IInitializable.h"

using namespace std;

#define DEFAULT_FUNCTIONAL true
//#define DEFAULT_FUNCTIONAL false

class EventHandler : public ProgramComponent, public IInitializable
{
public:
	bool FUNCTIONAL;
	EventHandler();
	~EventHandler(void);

	virtual void initialize(int _Argc, void** _Argv)=0;
	virtual void treatEvent(int _Args, void* _Argv[])=0;
};

#endif