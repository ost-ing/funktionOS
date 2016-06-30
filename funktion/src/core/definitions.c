#include "definitions.h"

E4C_DEFINE_EXCEPTION(InvalidArgumentException, "Invalid argument exception.", RuntimeException);
E4C_DEFINE_EXCEPTION(InitialisationException, "Initialisation exception.", RuntimeException);
E4C_DEFINE_EXCEPTION(AlreadyInitException, "Already initialised exception", InitialisationException);
E4C_DEFINE_EXCEPTION(IndexOutOfRangeException, "Index out of range exception.", RuntimeException);
E4C_DEFINE_EXCEPTION(MissingPointerException, "Missing pointer exception.", RuntimeException);
E4C_DEFINE_EXCEPTION(InvalidOperationException, "Invalid operation exception", RuntimeException);

void Regset(volatile uint32_t* reg, unsigned int bitfield, uint set)
{
	*reg = ((set != 0) ? (*reg | bitfield) : (*reg & ~bitfield));
}
