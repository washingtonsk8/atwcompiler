#ifndef	EventsManager_H
#define EventsManager_H

#include <vector>
#include "IInitializable.h"
#include "IDisposable.h"

using namespace std;

class EventHandler;

/*EventsManager
 *Funcionalidade - respons�vel por gerenciar exce��es de todo programa
 */
class EventsManager : public IInitializable, public IDisposable
{
public:
	static EventsManager* getInstance(){
		static EventsManager instance;
		return &instance;
	}

	void initialize(int _Argc, void** _Argv);
	void dispose(int _Argc, void** _Argv);

	//Construtor e destrutor padr�o
	~EventsManager(void);
	
	//Opera��es de get e set
	vector<vector<EventHandler*>> getHandlers();
	EventHandler*  getHandler(unsigned int GroupID, int ID);

	//Para opera��es basicas
	int registerHandler(unsigned int _GroupID, EventHandler* _Handler);
	int unregisterHandler(unsigned int _GroupID, EventHandler* _Handler);
	int resetHandlers();
	int callHandlers(unsigned int _GroupID, int _Args, void* _Argv[]);
	int notifyExHandler(unsigned int _GroupID, int _HandlerID, int _Args, void* _Argv[]);

private:
	EventsManager(void);	
	vector<vector<EventHandler*>> _eHandlers;
};
#endif
