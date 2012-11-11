#ifndef IINITIALIZABLE_H
#define IINITIALIZABLE_H
class IInitializable{
public:
	virtual void initialize(int _Argc, void** _Argv )=0;
};
#endif