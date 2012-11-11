#ifndef ATWSinEvents_H
#define ATWSinEvents_H

#include "EventHandler.h"
#include "Globals.h"

class ATWSinEvents: public EventHandler
{
public:
	ATWSinEvents(void);
	~ATWSinEvents(void);
	void initialize(int _Argc, void** _Argv);
	void treatEvent(int _Args, void* _Argv[]);
};
#endif
