// Driver de teclado.

#include "keyboard.h"

// Procura uma tecla pressionada. Se encontrada, a tecla ‚ removida
// do buffer de teclado e devolvida.
unsigned KeyEvent(void)
{
	unsigned E;
	E = bioskey(1);
	if (E != 0) {
		E = bioskey(0);
	}

	return E;
}

int IsShiftArrow(unsigned K)
{
	return (K == ShiftLeft) || (K == ShiftRight) ||
				 (K == ShiftUpKey) || (K == ShiftDnKey);
}
