#ifndef ProgramEvents_H
#define ProgramEvents_H

#include "EventHandler.h"
#include "Globals.h"

class ProgramEvents: public EventHandler
{
public:
	ProgramEvents(void);
	~ProgramEvents(void);
	void initialize(int _Argc, void** _Argv);
	void treatEvent(int _Args, void* _Argv[]);
};
#endif

