#include "msounit.h"

#include <stdio.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include "msounit.h"

Meso::Meso(char *N, ActionProc A) : Wso(0x00, 0x00, DefColors)
{
	strncpy(Name, N, 39);
	// Garante que a string termine com nulo.
	Name[39] = 0;
	Action = A;
	SetSize(Panel->TextWidth(N), Panel->TextHeight(1));
}

void Meso::Draw(void)
{
	Wso::Draw();
	Panel->HzWrt(0, 0, Name, Panel->Colors.Wc);
}

void Meso::Prompt(void)
{
	Wso::Prompt();
	Panel->Fill(0, 0, Panel->Interior->Wd, Panel->TextHeight(1), ' ',
							Panel->Colors.Fc);
	Panel->HzWrt(0, 0, Name, Panel->Colors.Fc);
}

void Meso::UnPrompt(void)
{
	Panel->Fill(0, 0, Panel->Interior->Wd, Panel->TextHeight(1), ' ',
							Panel->Colors.Wc);
	Panel->HzWrt(0, 0, Name, Panel->Colors.Wc);
	Wso::UnPrompt();
}

void Meso::OnKeyStroke(MsgPkt &M)
{
	if (M.Code == CrKey) {
		Activate(M);
	} else {
		Base->OnKeyStroke(M);
	}
}
