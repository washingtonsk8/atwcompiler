#ifndef	EventsManager_H
#define EventsManager_H

#include <vector>
#include "IInitializable.h"
#include "IDisposable.h"

using namespace std;

class EventHandler;

/*EventsManager
 *Funcionalidade - responsável por gerenciar exceções de todo programa
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

	//Construtor e destrutor padrão
	~EventsManager(void);
	
	//Operações de get e set
	vector<vector<EventHandler*>> getHandlers();
	EventHandler*  getHandler(unsigned int GroupID, int ID);

	//Para operações basicas
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
