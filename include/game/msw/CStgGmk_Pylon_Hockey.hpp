#ifndef MSW_CSTGGMK_PYLON_HOCKEY_HPP_
#define MSW_CSTGGMK_PYLON_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"
#include "game/msw/CStgGmk_Pylon_Act.hpp"
#include "game/msw/HockeyPrm.hpp"

namespace msw {

    // The pylons of hockey stage 04. The three classes are declared in the
    // reverse order of their vtables in
    // game/hockey/CStgGmk_Pylon_Act_Hockey.cpp's .data.

    // The actor of a pylon of hockey stage 04, size 0x210: two timers added
    // to msw::CStgGmk_Pylon_Act (0x208, 0x20C; the constructor clears both),
    // with the HOCKEY_PRM_S04_PYLON_* parameters. HockeyF's
    // game/hockey/CStgGmk_IceCube_Act_Hockey.cpp derives its ice cube actor
    // from it (its vtable keeps these overrides, and its constructor inlines
    // this one). The order follows __vt__Q23msw24CStgGmk_Pylon_Act_Hockey;
    // SetPushVel is the only new slot (0x2B4). The destructor is inline (the
    // ice cube's calls msw::CStgGmk_Pylon_Act's directly). The inline getters
    // are emitted in game/hockey/CStgGmk_Pylon_Act_Hockey.cpp after
    // CStgGmk_Pylon_Act's weak functions, in reverse order of these
    // declarations; the rest is out of line there.
    class CStgGmk_Pylon_Act_Hockey : public CStgGmk_Pylon_Act {
        public:
            CStgGmk_Pylon_Act_Hockey(CStgGmk *pParent)
                : CStgGmk_Pylon_Act(pParent), m_fReflectWait(0.0f), m_fHitSeWait(0.0f)
            {
            }
            virtual ~CStgGmk_Pylon_Act_Hockey() {}

            virtual void OnUpdate(float rate);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                               const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual float GetParam_MdlRadius() const { return HOCKEY_PRM_S04_PYLON_MDL_RADIUS; }
            virtual float GetParam_MdlHeight() const { return HOCKEY_PRM_S04_PYLON_MDL_HEIGHT; }
            virtual float GetParam_Radius() const { return HOCKEY_PRM_S04_PYLON_RADIUS; }
            virtual float GetParam_Height() const { return HOCKEY_PRM_S04_PYLON_HEIGHT; }
            virtual long GetParam_PushPower() const { return HOCKEY_PRM_S04_PYLON_PUSH_POWER; }
            virtual float GetParam_PushResist() const { return HOCKEY_PRM_S04_PYLON_PUSH_RESIST; }
            virtual float GetParam_Acc() const;
            virtual float GetParam_Dec() const;
            // The parameter is in seconds; the result in frames.
            virtual float GetParam_Interval() const { return 60.0f * HOCKEY_PRM_S04_PYLON_INTERVAL; }
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_MaxLife() const { return HOCKEY_PRM_S04_PYLON_LIFE; }
            virtual float GetParam_FieldMargin() const { return HOCKEY_PRM_S04_PYLON_FIELD_MARGIN; }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            // The ball bounces off hockey pylons (OnReflectHit).
            virtual bool IsEnableDefColl() const { return true; }
            virtual void SetPushVel(const bcs::FVEC3 &rVel); // 0x2B4

        private:
            // 0x208 (name guessed): frames during which further reflect
            // hits are ignored (8 frames after each).
            float m_fReflectWait;
            // 0x20C (name guessed): frames until a character's hit may play
            // its sound again (20 after each).
            float m_fHitSeWait;
    };

    // A pylon of hockey stage 04, size 0x78 (nothing added to
    // msw::CStgGmk_Pylon). Made by msw::CStgGmk_PylonCluster_Hockey::NewGmk.
    // All inline (HockeyF's ice cube's destructor inlines the destructor:
    // the null test comes twice); game/hockey/CStgGmk_Pylon_Act_Hockey.cpp
    // emits the vtable and NewAct, NewMdl and the destructor (reverse order
    // of the declarations here).
    class CStgGmk_Pylon_Hockey : public CStgGmk_Pylon {
        public:
            CStgGmk_Pylon_Hockey(bcs::CTask *pParent) : CStgGmk_Pylon(pParent, 0) {}
            virtual ~CStgGmk_Pylon_Hockey() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx) { return new (heap) CStgGmk_Pylon_Mdl(this, 0); }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Pylon_Act_Hockey(this); }
    };

    // The pylon cluster of the hockey mode, with the HOCKEY_PRM_S04_*
    // parameters. Size 0xA1C (nothing added to msw::CStgGmk_PylonCluster).
    // A member of msw::CStageHockey04, which inlines the constructor. The
    // other functions follow in game/hockey/CStgGmk_Pylon_Act_Hockey.cpp,
    // with the vtable and the weak destructor; NewGmk and
    // GetParam_LocPattern are inline (emitted there after
    // CStgGmk_PylonCluster's weak functions).
    class CStgGmk_PylonCluster_Hockey : public CStgGmk_PylonCluster {
        public:
            CStgGmk_PylonCluster_Hockey(bcs::CTask *pParent) : CStgGmk_PylonCluster(pParent) {}
            virtual ~CStgGmk_PylonCluster_Hockey() {}

            // HOCKEY_PRM_S04_PYLON_LOC_PAT_TYPE selects a kind of random
            // pattern (negated); zero means the fixed pattern
            // HOCKEY_PRM_S04_PYLON_LOC_PAT_NO.
            virtual long GetParam_LocPattern() const
            {
                if (HOCKEY_PRM_S04_PYLON_LOC_PAT_TYPE != 0) {
                    return -HOCKEY_PRM_S04_PYLON_LOC_PAT_TYPE;
                }
                return HOCKEY_PRM_S04_PYLON_LOC_PAT_NO;
            }
            // Declared after GetParam_LocPattern: the original emits it
            // before it (weak inlines come out in reverse declaration order).
            virtual CStgGmk *NewGmk(long heap) const { return new (heap) CStgGmk_Pylon_Hockey(GetParent()); }
            virtual long GetLocValidNumIdx() const;
            virtual long GetLocRawTblIdx(long pattern, long idx) const;
    };
}

#endif /* !MSW_CSTGGMK_PYLON_HOCKEY_HPP_ */
