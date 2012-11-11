#ifndef ILOADABLE_H
#define ILOADABLE_H
class ILoadable{
public:
	virtual void load(int _Argc, void** _Argv )=0;
};
#endif