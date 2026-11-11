#ifndef BCS_NEW_HPP_
#define BCS_NEW_HPP_

// The DOL's second allocation operator (__nw__FUll). What the long selects
// is not known; the callers seen so far pass 0 or 0x100.
void *operator new(unsigned long size, long nArg);

// The three-argument form (__nw__FUlll); msw::CStage17Base::NewTurnTable
// hands its two longs on (meaning not known, perhaps a heap and an
// alignment).
void *operator new(unsigned long size, long nArg, long nArg2);

// The array form (__nwa__FUll); bcs::CFsm allocates its state table with
// it, passing 0.
void *operator new[](unsigned long size, long nArg);

// (added for DOL game/bcs/CMiiMan.cpp) The array form of the three-argument
// one (__nwa__FUlll); bcs::CMiiMan passes 0x101 (or 1) and 0x20 or -0x20.
void *operator new[](unsigned long size, long nArg, long nArg2);

#endif /* !BCS_NEW_HPP_ */
