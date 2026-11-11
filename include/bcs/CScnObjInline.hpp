#ifndef BCS_CSCNOBJINLINE_HPP_
#define BCS_CSCNOBJINLINE_HPP_

#include "bcs/CScnObj.hpp"

// The bodies of bcs::CScnObj's inline virtual functions that are not in the
// class: SetAlphaMode, SetExBlendMode and the drawing and material hooks
// from slot 0x6C on, all empty but SetAlphaMode. Read from the weak copies
// MenuF's game/menu/CTopMenuChrObj.cpp emits with msw::CBBoxObj's vtable.
// Kept out of CScnObj.hpp for the same reason as game/msw/CBallInline.hpp:
// include it only from a unit whose original carries the copies.

inline void bcs::CScnObj::SetAlphaMode(AlphaMode mode)
{
    m_nAlphaMode = mode;
}

inline void bcs::CScnObj::SetExBlendMode(int)
{
}

inline void bcs::CScnObj::ExEffectDraw(int, unsigned long, void *)
{
}

inline void bcs::CScnObj::DrawJointObjOpa(void *)
{
}

inline void bcs::CScnObj::DrawJointObjXluExOpa(void *)
{
}

inline void bcs::CScnObj::DrawJointObjXluExXlu(void *)
{
}

inline void bcs::CScnObj::SetMatTevColor(_GXTevRegID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevColor(const char *, _GXTevRegID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevColorRGB(_GXTevRegID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevColorRGB(const char *, _GXTevRegID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevColorA(_GXTevRegID, unsigned char)
{
}

inline void bcs::CScnObj::SetMatTevColorA(const char *, _GXTevRegID, unsigned char)
{
}

inline void bcs::CScnObj::RestoreMatTevColor(_GXTevRegID)
{
}

inline void bcs::CScnObj::RestoreMatTevColor(const char *, _GXTevRegID)
{
}

inline void bcs::CScnObj::SetMatTevKColor(_GXTevKColorID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevKColor(const char *, _GXTevKColorID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevKColorRGB(_GXTevKColorID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevKColorRGB(const char *, _GXTevKColorID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatTevKColorA(_GXTevKColorID, unsigned char)
{
}

inline void bcs::CScnObj::SetMatTevKColorA(const char *, _GXTevKColorID, unsigned char)
{
}

inline void bcs::CScnObj::RestoreMatTevKColor(_GXTevKColorID)
{
}

inline void bcs::CScnObj::RestoreMatTevKColor(const char *, _GXTevKColorID)
{
}

inline void bcs::CScnObj::SetMatPixDstAlpha(unsigned char, unsigned char)
{
}

inline void bcs::CScnObj::SetMatPixDstAlpha(const char *, unsigned char, unsigned char)
{
}

inline void bcs::CScnObj::SetMatFogIdx(long)
{
}

inline void bcs::CScnObj::SetMatFogIdx(const char *, long)
{
}

inline void bcs::CScnObj::SetMatChanMatColor(_GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanMatColor(const char *, _GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanMatColorRGB(_GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanMatColorRGB(const char *, _GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanMatColorA(_GXChannelID, unsigned char)
{
}

inline void bcs::CScnObj::SetMatChanMatColorA(const char *, _GXChannelID, unsigned char)
{
}

inline void bcs::CScnObj::RestoreMatChanMatColor(_GXChannelID)
{
}

inline void bcs::CScnObj::RestoreMatChanMatColor(const char *, _GXChannelID)
{
}

inline void bcs::CScnObj::SetMatChanAmbColor(_GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanAmbColor(const char *, _GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanAmbColorRGB(_GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanAmbColorRGB(const char *, _GXChannelID, BCS_Color)
{
}

inline void bcs::CScnObj::SetMatChanAmbColorA(_GXChannelID, unsigned char)
{
}

inline void bcs::CScnObj::SetMatChanAmbColorA(const char *, _GXChannelID, unsigned char)
{
}

inline void bcs::CScnObj::RestoreMatChanAmbColor(_GXChannelID)
{
}

inline void bcs::CScnObj::RestoreMatChanAmbColor(const char *, _GXChannelID)
{
}

#endif /* !BCS_CSCNOBJINLINE_HPP_ */
