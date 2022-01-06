#ifndef H_TWSOUNIT
#define H_TWSOUNIT

// Objeto de janela de texto.

#include "isounit.h"
#include "tfsounit.h"

class Twso : public Iso
{
public:
	Twso(int Ba, int Fa, ColorPack &Cp);
	virtual void MoveLoop(MsgPkt &M);
	virtual void StretchLoop(MsgPkt &M);
	virtual void Prompt(void);
	virtual void UnPrompt(void);
};

#endif
