#ifndef	ATWLexException_H
#define ATWLexException_H

#include "EventHandler.h"
#include "Globals.h"

class ATWLexEvents : public EventHandler
{
public:
	ATWLexEvents(void);
	~ATWLexEvents(void);
	void initialize(int _Argc, void** _Argv);
	void treatEvent(int _Args, void* _Argv[]);
};

#endif