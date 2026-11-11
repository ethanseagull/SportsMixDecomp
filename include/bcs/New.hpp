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

#endif /* !BCS_NEW_HPP_ */
