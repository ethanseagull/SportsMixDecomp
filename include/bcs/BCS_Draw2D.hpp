#ifndef BCS_BCS_DRAW2D_HPP_
#define BCS_BCS_DRAW2D_HPP_

#include "bcs/BCS_Color.hpp"

// Helpers for drawing flat shapes over the screen with GX, in the DOL at
// 0x80071380 and on (all unnamed there). The file name, the function names
// and the parameter meanings are guesses, read from their bodies and from
// msw::CGameExDemoMaskObj (game/msw/CGameExDemoBase.cpp).

namespace bcs {

    // (fn_800714BC) Viewport, scissor and an orthographic projection for
    // view nView, or for the whole screen in pixels when nView is negative.
    void Draw2D_SetProjection(long nView, int);
    // (fn_8007195C) No depth test or update: GXSetZMode(0, GX_ALWAYS, 0).
    void Draw2D_SetZModeOff();
    // (fn_80071708) One colour channel, lit by the vertex colour.
    void Draw2D_SetChanVtxColor();
    // (fn_80071920) No blending: GXSetBlendMode(GX_BM_NONE, ...).
    void Draw2D_SetBlendNone();
    // (fn_80071A08) A filled rectangle of colour `color`.
    void Draw2D_FillRect(short x, short y, short w, short h, BCS_Color color);
}

#endif /* !BCS_BCS_DRAW2D_HPP_ */
