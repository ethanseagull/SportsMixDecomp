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

            // INCOMPLETE (DOL class; added for HockeyF
            // game/hockey/CChrCoreHockeySlime.cpp). One of the three slimes
            // of the Slime's special shot: it circles a point, then chases
            // the nearest player of the other team. 0x118 bytes (HockeyF's
            // CC16_HOCKEY::CcSlimeSquadHockey is allocated at 0x11C and adds
            // one word; BasketF's
            // CC16_BASKET::CcSlimeSquadBasket is allocated at 0x118 and adds
            // none). A bcs::CFsm of its own states is at 0x94 (vptr at
            // 0xD8; the @148@ thunk). The virtual functions are read from
            // __vt__Q33msw11CC16_HOCKEY18CcSlimeSquadHockey; return types and
            // argument meanings guessed. Members mapped only where the
            // module code uses them.
            class CcSlimeSquad : public CChrObstacle, public bcs::CFsm<CcSlimeSquad> {
                public:
                    // nIndex: which of the three slimes (0 to 2); nTeam and
                    // nPlayer: the shooter's team and player numbers.
                    CcSlimeSquad(bcs::CTask *pParent, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, long nIndex,
                                 long nTeam, long nPlayer);
                    virtual ~CcSlimeSquad();
                    virtual void Main();
                    virtual long SendMessage(long, long, long, long);
                    virtual float GetAttackRadius() const;
                    virtual float GetAttackRange() const;
                    virtual int IsAttackEnd() const;

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
                    // The motion id of motion nMot of this slime.
                    virtual short GetMotID_(long nMot) const;
                    virtual float GetRotateRadius_() const;
                    virtual float GetRotateRotSpeed_() const;
                    virtual float GetRotateSpeed_() const;
                    virtual float GetChaseSpeed_() const;
                    virtual float GetSearchTargetDist_() const;
                    virtual float GetGiveupChaseDist_() const;

                    // Makes the model from res with the motions of mot.
                    void Create(nw4r::g3d::ResFile res, nw4r::g3d::ResFile mot);
                    // Sends the slime round rCenter at height fY, starting
                    // at the yaw fRotY (guess).
                    void SetTargetOrbit(const bcs::FVEC3 &rCenter, float fY, float fRotY);

                protected:
                    bcs::CMdlObj *m_pMdl;        // 0xDC
                    unsigned char m_UnkE0[0xEC - 0xE0];
                    CBodyCollision m_Body;       // 0xEC
                    unsigned char m_UnkF8[0x100 - 0xF8];
                    long m_nIndex;               // 0x100, the constructor's nIndex
                    unsigned char m_Unk104[0x108 - 0x104];
                    unsigned long m_hEffect;     // 0x108, an effect attached to the model
                    unsigned char m_Unk10C[0x110 - 0x10C];
                    long m_nTeam;                // 0x110, the constructor's nTeam
                    long m_nPlayer;              // 0x114, the constructor's nPlayer
            };
        }
    }
}

#endif /* !MSW_CC_CMN_SLIME_CCSLIMESQUAD_HPP_ */
