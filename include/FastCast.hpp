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

#endif /* !FASTCAST_HPP_ */
