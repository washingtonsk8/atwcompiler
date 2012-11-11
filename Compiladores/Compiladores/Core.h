#ifndef CORE_H
#define CORE_H

#include "FileManager.h"
#include "EventsManager.h"
#include "ProgramComponent.h"
#include "IInitializable.h"
#include "IDisposable.h"

class Core : public ProgramComponent, public IInitializable, public IDisposable
{
public:
	Core(void);
	~Core(void);

	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);

	FileManager* getFileManager();
	EventsManager* getEventsManager();

private:
	FileManager* _fManager;
};
#endif
