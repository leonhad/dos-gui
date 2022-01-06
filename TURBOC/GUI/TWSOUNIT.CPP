#include "twsounit.h"

Twso::Twso(int Ba, int Fa, ColorPack &Cp) : Iso(new Tfso(Ba, Fa, Cp))
{
}

void Twso::MoveLoop(MsgPkt &M)
{
	Tskel *Red;
	Iso *Skeleton;

	if (Panel->IsSwappable()) {
		if ((Panel->Fattr & OutlineMove) != 0) {
			Red = new Tskel(Panel->Colors);
			Red->SetSize(Panel->Interior->Wd, Panel->Interior->Ht);
			Skeleton = new Iso(Red);
			Skeleton->Open(Base, Panel->Frame->Xul - Base->Panel->Interior->Xul,
										 Panel->Frame->Yul - Base->Panel->Interior->Yul);
			Skeleton->MoveLoop(M);
			Skeleton->Remove();

			if ((Panel->Frame->Xul != Skeleton->Panel->Frame->Xul) ||
					(Panel->Frame->Yul != Skeleton->Panel->Frame->Yul)) {
				Move(Skeleton->Panel->Frame->Xul,
						 Skeleton->Panel->Frame->Yul);
			}

			delete Skeleton;
		} else {
			Iso::MoveLoop(M);
		}
	}
}

void Twso::StretchLoop(MsgPkt &M)
{
	Tskel *Red;
	Iso *Skeleton;

	if (Panel->IsStretchable()) {
		if ((Panel->Fattr & OutlineMove) != 0) {
			Red = new Tskel(Panel->Colors);
			Red->SetSize(Panel->Interior->Wd, Panel->Interior->Ht);
			Skeleton = new Iso(Red);
			Skeleton->Open(Base, Panel->Frame->Xul - Base->Panel->Interior->Xul,
										 Panel->Frame->Yul - Base->Panel->Interior->Yul);
			Skeleton->StretchLoop(M);
			Skeleton->Remove();

			if ((Panel->Frame->Wd != Skeleton->Panel->Frame->Wd) ||
					(Panel->Frame->Ht != Skeleton->Panel->Frame->Ht)) {
				Stretch(Skeleton->Panel->Frame->Wd - 2,
								Skeleton->Panel->Frame->Ht - 2);
			}

			delete Skeleton;
		} else {
			Iso::StretchLoop(M);
		}
	}
}

void Twso::Prompt(void)
{
	Iso::Prompt();

	if ((Panel->Bstyle > 0) && ((Panel->Fattr & BorderPrompt) != 0)) {
		Panel->DrawFrame(Double + Panel->Bwd, 0);
	}
}

void Twso::UnPrompt(void)
{
	if (Active) {
		if ((Panel->Bstyle > 0) &&
				((Panel->Fattr & BorderPrompt) != 0)) {
			Panel->DrawFrame(0, 0);
		}

		Iso::UnPrompt();
	}
}
