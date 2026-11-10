#ifndef BCS_NEW_HPP_
#define BCS_NEW_HPP_

// The DOL's second allocation operator (__nw__FUll). What the long selects
// is not known; the callers seen so far pass 0 or 0x100.
void *operator new(unsigned long size, long nArg);

// The array form (__nwa__FUll); bcs::CFsm allocates its state table with
// it, passing 0.
void *operator new[](unsigned long size, long nArg);

#endif /* !BCS_NEW_HPP_ */
