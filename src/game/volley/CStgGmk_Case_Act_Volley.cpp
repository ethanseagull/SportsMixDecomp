// The actor of the cases on the ship stage (stage 15) in volleyball:
// msw::CStgGmk_Case_Act_Volley. The unit is named after the weak
// msw::CStgSeSlot functions at its end (the cutter took them for a file of
// their own); the actor's functions come first, then the weak inline
// functions of CStgSeSlot, CStgAct, CStgGmkAct, CStgGmk_Case_Act and the
// actor's own parameter getters, and the this-adjusting thunks. Its .data
// is the actor's vtable.
#include "game/msw/CStgGmk_Case_Volley.hpp"
#include "game/msw/CBall.hpp"

long msw::CStgGmk_Case_Act_Volley::GetParam_Effect(long type) const
{
    switch (type) {
        case 0:
            return 0x6C5;
        case 1:
            return 0x6C6;
        case 2:
            return 0xC;
    }
    return -1;
}

long msw::CStgGmk_Case_Act_Volley::GetParam_SE(long type) const
{
    switch (type) {
        case 0:
            return 0xD7E;
        case 1:
            return 0xD7C;
        case 3:
            return 0xD7D;
    }
    return CStgGmk_Case_Act::GetParam_SE(type);
}

void msw::CStgGmk_Case_Act_Volley::UpdateColl()
{
    CStgGmk_Case_Act::UpdateColl();
}

// Any hit breaks a case that is not idle.
unsigned long msw::CStgGmk_Case_Act_Volley::OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                         const bcs::FVEC3 &rPos)
{
    if (GetState() != 0) {
        FsmqChangeState(3);
    }
    return 2;
}

// A shot ball caught by the case breaks it; the ball loses its hit mode.
void msw::CStgGmk_Case_Act_Volley::OnBallCatch(CCollBodyUnit *pBody, CBall *pBall)
{
    if (GetState() != 3 && pBall->IsShootMode()) {
        FsmqChangeState(3);
    }
    pBall->SetVolleyAnyHitMode(0, 0);
}

msw::CStgGmk_Case_Act_Volley::~CStgGmk_Case_Act_Volley()
{
}
