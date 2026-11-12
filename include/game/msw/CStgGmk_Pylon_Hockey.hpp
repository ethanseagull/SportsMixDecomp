#ifndef MSW_CSTGGMK_PYLON_HOCKEY_HPP_
#define MSW_CSTGGMK_PYLON_HOCKEY_HPP_

#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"
#include "game/msw/CStgGmk_Pylon_Act.hpp"

namespace msw {

    // The actor of a pylon of hockey stage 04, size 0x210: two floats added
    // to msw::CStgGmk_Pylon_Act (0x208, 0x20C; names and meanings not known
    // yet, the constructor clears both). Declared for HockeyF's
    // game/hockey/CStgGmk_IceCube_Act_Hockey.cpp, whose ice cube actor
    // derives from it (its vtable keeps these overrides, and its
    // constructor inlines this one). The functions are in
    // game/hockey/CStgGmk_Pylon_Act_Hockey.cpp (not written yet). The order
    // follows __vt__Q23msw24CStgGmk_Pylon_Act_Hockey; SetPushVel is the only
    // new slot (0x2B4). The destructor is inline (the ice cube's calls
    // msw::CStgGmk_Pylon_Act's directly). Several getters are weak in that
    // unit, so inline in the original; declared here without bodies until
    // it is written.
    class CStgGmk_Pylon_Act_Hockey : public CStgGmk_Pylon_Act {
        public:
            CStgGmk_Pylon_Act_Hockey(CStgGmk *pParent)
                : CStgGmk_Pylon_Act(pParent), m_Unk208(0.0f), m_Unk20C(0.0f)
            {
            }
            virtual ~CStgGmk_Pylon_Act_Hockey() {}

            virtual void OnUpdate(float rate);
            virtual unsigned long OnBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                               const bcs::FVEC3 &rPos);
            virtual void OnBallCatch(CCollBodyUnit *pBody, CBall *pBall);
            virtual unsigned long OnReflectHit(const CCollDefUnit *pDef, const CBall *pBall, bcs::FVEC3 &rPos,
                                               bcs::FVEC3 &rVel);
            virtual float GetParam_MdlRadius() const;
            virtual float GetParam_MdlHeight() const;
            virtual float GetParam_Radius() const;
            virtual float GetParam_Height() const;
            virtual long GetParam_PushPower() const;
            virtual float GetParam_PushResist() const;
            virtual float GetParam_Acc() const;
            virtual float GetParam_Dec() const;
            virtual float GetParam_Interval() const;
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_MaxLife() const;
            virtual float GetParam_FieldMargin() const;
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual bool IsEnableDefColl() const;
            virtual void SetPushVel(const bcs::FVEC3 &rVel); // 0x2B4

        private:
            float m_Unk208; // 0x208
            float m_Unk20C; // 0x20C
    };

    // A pylon of hockey stage 04, size 0x78 (nothing added to
    // msw::CStgGmk_Pylon). The destructor is inline (HockeyF's ice cube's
    // destructor inlines it: the null test comes twice); NewMdl and NewAct
    // are weak in game/hockey/CStgGmk_Pylon_Act_Hockey.cpp, declared here
    // without bodies until that unit is written.
    class CStgGmk_Pylon_Hockey : public CStgGmk_Pylon {
        public:
            CStgGmk_Pylon_Hockey(bcs::CTask *pParent) : CStgGmk_Pylon(pParent, 0) {}
            virtual ~CStgGmk_Pylon_Hockey() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx);
            virtual CStgAct *NewAct(long heap);
    };

    // The pylon cluster of the dodgeball mode, with the HOCKEY_PRM_S04_*
    // parameters. Size 0xA1C (nothing added to msw::CStgGmk_PylonCluster).
    // A member of msw::CStageHockey04, which inlines the constructor and
    // emits the weak destructor. The other functions follow in DodgeF
    // (game/hockey/CStgGmk_Pylon_Act_Hockey.cpp), with the vtable.
    class CStgGmk_PylonCluster_Hockey : public CStgGmk_PylonCluster {
        public:
            CStgGmk_PylonCluster_Hockey(bcs::CTask *pParent) : CStgGmk_PylonCluster(pParent) {}
            virtual ~CStgGmk_PylonCluster_Hockey() {}

            virtual CStgGmk *NewGmk(long heap) const;
            virtual long GetParam_LocPattern() const;
            virtual long GetLocValidNumIdx() const;
            virtual long GetLocRawTblIdx(long, long) const;
    };
}

#endif /* !MSW_CSTGGMK_PYLON_HOCKEY_HPP_ */
