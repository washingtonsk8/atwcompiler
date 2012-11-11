#ifndef PROGRAMCOMPONENT_H
#define PROGRAMCOMPONENT_H

#include "EventsManager.h"

class ProgramComponent
{
public:
	ProgramComponent(void);
	~ProgramComponent(void);

	int setGroupID(int _value);
	int getGroupID();
	int setID(int _value);
	int getID();

private:
	int _GroupID;
	int _ID;

protected:
	EventsManager* _eManager;
};
#endif

