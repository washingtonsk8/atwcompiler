#ifndef	ATW_SEMEVENTS_H
#define ATW_SEMEVENTS_H

#include "EventHandler.h"
#include "Globals.h"

class ATWSemEvents: public EventHandler
{
public:
	ATWSemEvents(void);
	~ATWSemEvents(void);
	void initialize(int _Argc, void** _Argv);
	void treatEvent(int _Args, void* _Argv[]);
};
#endif

