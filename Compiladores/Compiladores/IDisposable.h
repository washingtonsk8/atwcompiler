#ifndef IDISPOSABLE_H
#define IDISPOSABLE_H
class IDisposable{
public:
	virtual void dispose(int _Argc, void** _Argv)=0;
};
#endif