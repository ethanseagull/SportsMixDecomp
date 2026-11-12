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
    // (fn_80071BB8, name guessed; added for the strap screen in
    // game/bcs/CTask.cpp) Draw2D_FillRect with alpha blending, setting up
    // its own projection.
    void Draw2D_FillRectAlpha(short x, short y, short w, short h, BCS_Color color);
    // (fn_80071DA0, name guessed; added for the strap screen) The loaded
    // texture on the rectangle (x, y, w, h), texture coordinates (s0, t0)
    // to (s1, t1); the two ints are not known (called with 0, 0).
    void Draw2D_DrawTexRect(short x, short y, short w, short h, BCS_Color color, float s0, float t0, float s1,
                            float t1, int nUnk0, int nUnk1);
    // Added for MenuF game/menu/CBgDisplay.cpp: draws the loaded texture on
    // the screen rectangle (x, y, w, h) with texture coordinates (s0, t0)
    // to (s1, t1); the two ints are not known (called with 0, 1).
    void BCS_GX_DrawSpriteEx(short x, short y, short w, short h, BCS_Color color, float s0, float t0, float s1,
                             float t1, int nUnk0, int nUnk1);
    // (fn_80071934, name guessed) Alpha blending:
    // GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA).
    void Draw2D_SetBlendAlpha();

    struct FVEC3;

    // (fn_80072378 and fn_80072780, names guessed from their neighbours
    // BCS_GX_DrawLineEx and BCS_GX_DrawQuadEx) A line from (x0, y0, z0) to
    // (x1, y1, z1) shaded from c0 to c1, and a quad with a colour per corner,
    // in the current matrix. Added for the DOL's pinball ball.
    void BCS_GX_DrawLine(float x0, float y0, float z0, float x1, float y1, float z1, BCS_Color c0, BCS_Color c1);
    void BCS_GX_DrawQuad(const FVEC3 &rP0, const FVEC3 &rP1, const FVEC3 &rP2, const FVEC3 &rP3, BCS_Color c0,
                         BCS_Color c1, BCS_Color c2, BCS_Color c3);
    // (DOL, added for VolleyF's CStageVolley10::DrawEdgeOne_) The same two
    // under their own names; what the Ex adds is not known.
    void BCS_GX_DrawLineEx(float x0, float y0, float z0, float x1, float y1, float z1, BCS_Color c0, BCS_Color c1);
    void BCS_GX_DrawQuadEx(const FVEC3 &rP0, const FVEC3 &rP1, const FVEC3 &rP2, const FVEC3 &rP3, BCS_Color c0,
                           BCS_Color c1, BCS_Color c2, BCS_Color c3);
}

#endif /* !BCS_BCS_DRAW2D_HPP_ */
