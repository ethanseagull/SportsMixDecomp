#ifndef FASTCAST_HPP_
#define FASTCAST_HPP_

// Conversion of a signed halfword in memory to float through a quantized
// paired-single load, as the original does (msw::CEffectTable's and
// msw::CEffectAttachTable's getters: `psq_l f1, off(r3), 1, qr5`). It relies
// on GQR5 being set up for signed 16-bit values with scale 0, which the
// system does at start-up. Plain C does not give that instruction (the
// compiler converts through the 0x43300000 double trick), so the Wii build
// uses a paired-single load, like the bcs vector helpers; other compilers
// get the plain conversion, which gives the same value.
// Written for this project; not the SDK's code.
static inline float S16ToF32(register const short *pIn)
{
#ifdef __MWERKS__
    register float ret;
    asm { psq_l ret, 0(pIn), 1, 5 }
    return ret;
#else
    return (float)*pIn;
#endif
}

// The same for an unsigned halfword through GQR3 (unsigned 16-bit, scale
// 0): bcs::CScnObj::GetAlphaRateF in the DOL widens its byte to a halfword
// on the stack and loads it with `psq_l f2, 0(r3), 1, qr3`.
static inline float U16ToF32(register const unsigned short *pIn)
{
#ifdef __MWERKS__
    register float ret;
    asm { psq_l ret, 0(pIn), 1, 3 }
    return ret;
#else
    return (float)*pIn;
#endif
}

// Float to signed halfword through a quantized paired-single store to a
// stack word and a halfword load back (GQR5, signed 16-bit, scale 0; the
// value saturates instead of wrapping), as bcs::CScnObj::SetAlphaRateF in
// the DOL does: `psq_st f0, 0(r4), 1, qr5` then `lha`. The result goes
// through *pOut, which is why the original stores it to a second stack
// slot.
static inline void F32ToS16(register float in, short *pOut)
{
#ifdef __MWERKS__
    register short ret;
    unsigned long tmp;
    register unsigned long *pTmp = &tmp;
    asm {
        psq_st in, 0(pTmp), 1, 5
        lha ret, 0(pTmp)
    }
    *pOut = ret;
#else
    *pOut = (short)(in < -32768.0f ? -32768.0f : (in > 32767.0f ? 32767.0f : in));
#endif
}

// The same to an unsigned halfword through GQR3 (unsigned 16-bit, scale 0),
// as MenuF's SelectMenu::CNwLayout::SetRestoreBuffer does with a pane's
// size: `psq_st f1, 0(r4), 1, qr3` then `lhz`.
static inline void F32ToU16(register float in, unsigned short *pOut)
{
#ifdef __MWERKS__
    register unsigned short ret;
    unsigned long tmp;
    register unsigned long *pTmp = &tmp;
    asm {
        psq_st in, 0(pTmp), 1, 3
        lhz ret, 0(pTmp)
    }
    *pOut = ret;
#else
    *pOut = (unsigned short)(in < 0.0f ? 0.0f : (in > 65535.0f ? 65535.0f : in));
#endif
}

#endif /* !FASTCAST_HPP_ */
