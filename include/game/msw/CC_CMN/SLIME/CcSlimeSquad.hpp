#ifndef MSW_CC_CMN_SLIME_CCSLIMESQUAD_HPP_
#define MSW_CC_CMN_SLIME_CCSLIMESQUAD_HPP_

#include "bcs/CFsm.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {
    namespace CC_CMN {
        namespace SLIME {

            // (DOL class; meaning guessed from the names) One of the three
            // slimes of the Slime's special shot: it circles the goal, then
            // chases characters of the other team. 0x118 bytes (BasketF
            // allocates its CC16_BASKET::CcSlimeSquadBasket, which adds no
            // member, with that size).
            //
            // INCOMPLETE: the bases and virtual functions are read from the
            // DOL's vtable lbl_804836D0 and BasketF's
            // __vt__Q33msw11CC16_BASKET18CcSlimeSquadBasket: the state
            // machine at 0x94 (its vptr at 0xD8), then the states and the
            // per-game hooks the modules override. Members are mapped from
            // the DOL constructor only where the modules read them; names
            // guessed.
            class CcSlimeSquad : public CChrObstacle, public bcs::CFsm<CcSlimeSquad> {
                public:
                    // At rPos turned by rRot; nIndex is which of the three
                    // (0 to 2), nTeam and nPlayer the owner's.
                    CcSlimeSquad(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nIndex,
                                 long nTeam, long nPlayer);
                    virtual ~CcSlimeSquad();
                    virtual void Main();
                    virtual long SendMessage(long, long, long, long);
                    virtual float GetAttackRadius() const;
                    virtual float GetAttackRange() const;
                    virtual int IsAttackEnd() const;

                    // Makes the model of res with the motions of mot.
                    void Create(nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot);
                    // Starts circling round rCenter at height fHeight, from
                    // angle fRot. (meanings guessed)
                    void SetTargetOrbit(const bcs::FVEC3 &rCenter, float fHeight, float fRot);

                    virtual void MainFunc();
                    virtual void CtorStateFire();
                    virtual void LoopStateFire();
                    virtual void DtorStateFire();
                    virtual void CtorStateTouchDown();
                    virtual void LoopStateTouchDown();
                    virtual void DtorStateTouchDown();
                    virtual void CtorStateIdle();
                    virtual void LoopStateIdle();
                    virtual void DtorStateIdle();
                    virtual void CtorStateChase();
                    virtual void LoopStateChase();
                    virtual void DtorStateChase();
                    virtual void CtorStateDead();
                    virtual void LoopStateDead();
                    virtual void DtorStateDead();
                    // The motion of kind nKind (0 to 3) for this slime.
                    virtual long GetMotID_(long nKind) const;
                    virtual float GetRotateRadius_() const;
                    virtual float GetRotateRotSpeed_() const;
                    virtual float GetRotateSpeed_() const;
                    virtual float GetChaseSpeed_() const;
                    virtual float GetSearchTargetDist_() const;
                    virtual float GetGiveupChaseDist_() const;

                protected:
                    bcs::CMdlObj *m_pMdl;        // 0xDC
                    unsigned char m_UnkE0[0xEC - 0xE0];
                    CBodyCollision m_BodyColl;   // 0xEC
                    unsigned char m_UnkF8[0x100 - 0xF8];
                    long m_nIndex;               // 0x100, which of the three (0 to 2)
                    unsigned char m_Unk104[0x108 - 0x104];
                    // 0x108: the effect attached to the model (the modules
                    // set and stop it), 0 for none.
                    unsigned long m_hEffect;
                    unsigned char m_Unk10C[0x110 - 0x10C];
                    long m_nTeam;                // 0x110
                    long m_nPlayer;              // 0x114
            };
        }
    }
}

#endif /* !MSW_CC_CMN_SLIME_CCSLIMESQUAD_HPP_ */
