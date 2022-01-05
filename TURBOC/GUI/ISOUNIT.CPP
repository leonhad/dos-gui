#include "keyboard.h"
#include "isounit.h"
#include "msmouse.h"

MsgPkt NullMsg = {
	NULL,
	Idle,
	Idle,
	0,
	0,
	""
};

Iso::Iso(Fso *P)
{
	Panel = P;
	Active = false;
	Visible = false;
	IsClosed = true;
	TouchFlag = false;
	ClipToFrame = false;
	Under = NULL;
	Over = NULL;
	Base = NULL;
	SubMgr = new IsoMgr(this);
}

Iso::~Iso(void)
{
	Remove();
	delete SubMgr;
	delete Panel;
}

Rso *Iso::ClippingRect(void)
{
	if (ClipToFrame) {
		return Base->Panel->Frame;
	}

	return Base->Panel->Interior;
}

void Iso::SetLocn(int X, int Y, CoordType Ctype)
{
	int Dx;
	int Dy;
	int Dw;
	int Dh;
	Iso *P;
	Rso *Clipper;

	if (Base == NULL) {
		Panel->SetLocn(X, Y);
	} else {
		Clipper = ClippingRect();
		if (Ctype == Relc) {
			X += Clipper->Xul;
			Y += Clipper->Yul;
		}

		Dw = Panel->Frame->Wd;
		Dh = Panel->Frame->Ht;
		Clipper->ClipDim(X, Y, Dw, Dh);

		Dx = X - Panel->Frame->Xul;
		Dy = Y - Panel->Frame->Yul;

		Panel->SetLocn(X, Y);

		P->SubMgr->Top;
		while(P != NULL) {
			P->SetLocn(P->Panel->Frame->Xul + Dx, P->Panel->Frame->Yul + Dy, Absc);
			P = P->Under;
		}
	}
}

void Iso::SetSize(int W, int H)
{
	int Dw;
	int Dh;
	int Ow;
	int Oh;
	Rso *Clipper;
	Rso *Pf = Panel->Frame;

	Panel->SetSize(W, H);

	if (Base != NULL) {
		Clipper = ClippingRect();
		Pf = Panel->Frame;
		Ow = Pf->Wd;
		Oh = Pf->Ht;

		Clipper->ClipSize(Pf->Wd, Pf->Ht);

		Dw = Pf->Wd - Ow;
		Dh = Pf->Ht - Oh;

		if ((Dw != 0) || (Dh != 0)) {
			Panel->SetSize(Panel->Interior->Wd + Dw,
										 Panel->Interior->Ht + Dh);
		}
	}
}

void Iso::DrawPanel(void)
{
	Mouse.Hide();
	Visible = true;

	if (Panel->IsSwappable()) {
		Panel->GetImage(ClippingRect());
	}

	Panel->DrawFrame(0, 0);
	Panel->DrawShadows(ClippingRect(), GetIm, 1);
	Panel->Clear(' ', 0);
	Draw();
	Mouse.Show();
}

void Iso::Open(Iso *B, int X, int Y)
{
	Base = B;
	Base->SubMgr->Push(this);
	IsClosed = false;
	SetSize(Panel->Interior->Wd, Panel->Interior->Ht);
	SetLocn(X, Y, Relc);
	DrawPanel();
}

void Iso::Reopen(int X, int Y)
{
	IsClosed = false;
	SetLocn(X, Y, Relc);

	// Move o iso para frente e mostra-o.
	Select();
}

void Iso::Move(int X, int Y)
{
	if (Panel->IsSwappable()) {
		Hide();
		SetLocn(X, Y, Absc);
		Show();
	}
}

void Iso::DeltaMove(int Dx, int Dy)
{
	Move(Panel->Frame->Xul + Dx, Panel->Frame->Yul + Dy);
}

void Iso::MoveLoop(MsgPkt &M)
{
	unsigned E;

	// Reinicia o contador.
	Mouse.Moved();
	do {
		if (Mouse.Moved()) {
			DeltaMove(Mouse.Dx, Mouse.Dy);
		}

		E = Mouse.Event(M.Mx, M.My);
	} while(E != MouseUp);

	M.RtnCode = Idle;
}

void Iso::Stretch(int W, int H)
{
	Iso *P;
	int Mw;
	int Mh;

	if (Panel->IsStretchable()) {
		// M¡nimo absoluto.
		Mw = 8;
		Mh = 3;

		P = SubMgr->Top;
		while(P != NULL) {
			if (P->Panel->Frame->Wd > Mw) {
				Mw = P->Panel->Frame->Wd;
			}

			if (P->Panel->Frame->Ht > Mh) {
				Mh = P->Panel->Frame->Ht;
			}

			P->Hide();
			P = P->Under;
		}

		// Ajusta os tamanhos para o limite inferior.
		if (W < Mw) {
			W = Mw;
		}

		if (H < Mh) {
			H = Mh;
		}

		Hide();
		SetSize(W, H);
		SetLocn(Panel->Frame->Xul, Panel->Frame->Yul, Absc);
		Redraw();

		P = SubMgr->Bottom;
		// Mostra novamente as janelas.
		while(P != NULL) {
			P->SetLocn(P->Panel->Frame->Xul, P->Panel->Frame->Yul, Absc);
			if (!P->IsClosed) {
				P->Show();
			}

			P = P->Over;
		}
	}
}

void Iso::DeltaStretch(int Dw, int Dh)
{
	Stretch(Panel->Interior->Wd + Dw, Panel->Interior->Ht + Dh);
}

void Iso::StretchLoop(MsgPkt &M)
{
	unsigned E;

	// Reinicia o contador.
	Mouse.Moved();
	do {
		if (Mouse.Moved()) {
			DeltaStretch(Mouse.Dx, Mouse.Dy);
		}

		E = Mouse.Event(M.Mx, M.My);
	} while(E != MouseUp);

	M.RtnCode = Idle;
}

void Iso::Swap(void)
{
	XfrDirn Xd;

	if (Visible) {
		Xd = PutIm;
	} else {
		Xd = GetIm;
	}

	if (Base != NULL) {
		Panel->Swap(Base->Panel->Interior, Xd);
	} else {
		Panel->Swap(Panel->Overall, Xd);
	}
}

void Iso::Hide(void)
{
	if (IsClosed) {
		return;
	}

	if (Visible) {
		Swap();
		SetVisibleFlag(false);
	}
}

void Iso::Show(void)
{
	if (IsClosed) {
		return;
	}

	if (!Visible) {
		Swap();
		SetVisibleFlag(true);
	}
}

void Iso::SetVisibleFlag(int F)
{
	Iso *P;

	if (IsClosed) {
		return;
	}

	P = SubMgr->Top;
	while(P != NULL) {
		P->SetVisibleFlag(F);
		P = P->Under;
	}

	Visible = F;
}

void Iso::Select(void)
{
	if (Base != NULL) {
		Base->Select();
		Base->SubMgr->MoveToFront(this, true);
	}

	Show();
}

void Iso::Remove(void)
{
	if (Base != NULL) {
		Base->SubMgr->MoveToFront(this, false);
	}

	Base = NULL;
}

void Iso::Prompt(void)
{
	Select();
	Active = true;
}

void Iso::UnPrompt(void)
{
	Active = false;
}

void Iso::SwitchFocus(MsgPkt &M)
{
	if (M.Focus != this) {
		if (M.Focus != NULL) {
			// Sai do foco antigo.
			M.Focus->Leave(M);
		}

		M.Focus = this;
		Enter(M);
	}

	M.RtnCode = Idle;
}

void Iso::Enter(MsgPkt &)
{
	if (!Active) {
		Prompt();
	}
}

void Iso::Leave(MsgPkt &M)
{
	if (Active) {
		UnPrompt();
	}

	if (M.Focus == this) {
		M.Focus = NULL;
	}
}

void Iso::OnMouseEnter(MsgPkt &M)
{
	if (Mouse.ButtonStatus() != 0) {
		SwitchFocus(M);
	}
}

void Iso::OnMouseLeave(MsgPkt &M)
{
	if (Mouse.ButtonStatus() != 0) {
		Leave(M);
	}
}

void Iso::OnClose(MsgPkt &M)
{
	Leave(M);

	if (Panel->IsCloseable()) {
		Hide();
		IsClosed = true;
	}
}

void Iso::OnMouseDown(MsgPkt &M)
{
	SwitchFocus(M);

	if (Panel->OnBorder(M.Mx, M.My)) {
		BorderHandler(M);
	}
}

void Iso::BorderHandler(MsgPkt &M)
{
	if (Panel->OnCloseButton(M.Mx, M.My)) {
		M.RtnCode = Close;
		M.Focus = this;
	} else if ((M.Mx == Panel->Frame->Xlr) &&
						 (M.My == Panel->Frame->Ylr) &&
						 (Panel->Frame->Wd > 1) &&
						 (Panel->Frame->Ht > 1)) {
		StretchLoop(M);
	} else {
		MoveLoop(M);
	}
}

void Iso::OnKeyStroke(MsgPkt &M)
{
	switch(M.Code) {
		case CrKey:
			Activate(M);
			M.Code = Idle;
			break;
		default:
			if (IsShiftArrow(M.Code)) {
				OnShiftArrow(M);
				M.Code = Idle;
			}
	}
}

void Iso::OnShiftArrow(MsgPkt &M)
{
	switch(M.Code) {
		case ShiftLeft:
			DeltaMove(-1, 0);
			break;
		case ShiftRight:
			DeltaMove(1, 0);
			break;
		case ShiftUpKey:
			DeltaMove(0, -1);
			break;
		case ShiftDnKey:
			DeltaMove(0, 1);
			break;
		default:
			break;
	}
}

void Iso::Dispatch(MsgPkt &M)
{
	M.RtnCode = Idle;

	switch(M.Code) {
		case Idle:
			break;
		case StrMsg:
			break;
		case Close:
			OnClose(M);
			break;
		case MouseDown:
			OnMouseDown(M);
			break;
		case MouseStillDown:
			OnMouseStillDown(M);
			break;
		case MouseUp:
			OnMouseUp(M);
			break;
		case MouseEnter:
			OnMouseEnter(M);
			break;
		case MouseLeave:
			OnMouseLeave(M);
			break;
		case MouseWithin:
			OnMouseWithin(M);
			break;
		default:
			OnKeyStroke(M);
			break;
	}
}

int Iso::Obscured(void)
{
	Iso *Ip = Over;
	while(Ip != NULL) {
		if (Panel->Touches(Ip->Panel)) {
			return true;
		}

		Ip = Ip->Over;
	}

	return false;
}

IsoMgr::IsoMgr(Iso *B)
{
	Top = NULL;
	Bottom = NULL;
	Base = B;
	Hot = NULL;
	Marker = NULL;
}

IsoMgr::~IsoMgr(void)
{
	Iso *P;
	Iso *Q;

	P = Top;
	while(P != NULL) {
		Q = P->Under;
		delete P;
		P = Q;
	}
}

void IsoMgr::Push(Iso *Ip)
{
	if (Top != NULL) {
		Top->Over = Ip;
		Ip->Under = Top;
	} else {
		Ip->Over = NULL;
		Ip->Under = NULL;
		Bottom = Ip;
		Marker = Ip;
	}

	Top = Ip;
}

void IsoMgr::MoveToFront(Iso *Me, int Keep)
{
	Iso *Ip;

	if (Keep && (Me == Top)) {
		return;
	}

	Mouse.Hide();
	if (Me->Panel->IsSwappable()) {
		ResetTouchFlags(Me);
		SetTouchFlags(Me);
		if (!Me->TouchFlag) {
			if (!Keep) {
				Me->Hide();
			}
		} else {
			Ip = Top;
			while(Ip != Me->Under) {
				if (Ip->TouchFlag) {
					Ip->Hide();
				}

				Ip = Ip->Under;
			}

			Ip = Me->Over;
			while(Ip != NULL) {
				if (Ip->TouchFlag) {
					Ip->Show();
				}

				Ip = Ip->Over;
			}
		}
	}

	if (Me == Top) {
		if (Me == Bottom) {
			Bottom = NULL;
			Top = NULL;
		} else {
			Me->Under->Over = NULL;
			Top = Me->Under;
		}

		Me->Under = NULL;
		Me->Over = NULL;
	} else {
		if (Me == Bottom) {
			Me->Over->Under = NULL;
			Bottom = Me->Over;
		} else {
			Me->Under->Over = Me->Over;
			Me->Over->Under = Me->Under;
		}

		if (Keep) {
			Top->Over = Me;
			Me->Under = Top;
			Me->Over = NULL;
			Top = Me;

			if (Me->TouchFlag) {
				Me->Show();
			}
		} else {
			Me->Under = NULL;
			Me->Over = NULL;
		}
	}

	Mouse.Show();
}
