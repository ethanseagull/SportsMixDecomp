#ifndef MSW_CSTGGMK_PYLON_DODGE_HPP_
#define MSW_CSTGGMK_PYLON_DODGE_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"
#include "game/msw/CStgGmk_Pylon_Act.hpp"
#include "game/msw/DodgePrm.hpp"

namespace msw {

    // The pylons of stage 04 in the dodgeball mode. The three classes are
    // declared in the reverse order of their vtables in
    // game/dodge/CStgGmk_Pylon_Act_Dodge.cpp's .data.

    // The actor, size 0x208 (nothing added to msw::CStgGmk_Pylon_Act), with
    // the DODGE_PRM_S04_PYLON_* parameters. The inline getters are emitted
    // after CStgGmk_Pylon_Act's weak functions, in reverse order of these
    // declarations; the rest is in game/dodge/CStgGmk_Pylon_Act_Dodge.cpp.
    class CStgGmk_Pylon_Act_Dodge : public CStgGmk_Pylon_Act {
        public:
            CStgGmk_Pylon_Act_Dodge(CStgGmk *pParent) : CStgGmk_Pylon_Act(pParent) {}
            virtual ~CStgGmk_Pylon_Act_Dodge() {}

            virtual float GetParam_MdlRadius() const { return DODGE_PRM_S04_PYLON_MDL_RADIUS; }
            virtual float GetParam_MdlHeight() const { return DODGE_PRM_S04_PYLON_MDL_HEIGHT; }
            virtual float GetParam_Radius() const { return DODGE_PRM_S04_PYLON_RADIUS; }
            virtual float GetParam_Height() const { return DODGE_PRM_S04_PYLON_HEIGHT; }
            virtual long GetParam_PushPower() const { return DODGE_PRM_S04_PYLON_PUSH_POWER; }
            virtual float GetParam_PushResist() const { return DODGE_PRM_S04_PYLON_PUSH_RESIST; }
            virtual float GetParam_Acc() const;
            virtual float GetParam_Dec() const;
            // Fifteen seconds, in frames.
            virtual float GetParam_Interval() const { return 900.0f; }
            virtual long GetParam_Effect(long type) const;
            virtual float GetParam_MaxLife() const { return DODGE_PRM_S04_PYLON_LIFE; }
            virtual float GetParam_FieldMargin() const { return DODGE_PRM_S04_PYLON_FIELD_MARGIN; }
            virtual void OnDamage(CCollBodyUnit *pBody, CBall *pBall);
            virtual void SetPushVel(const bcs::FVEC3 &rVel);
    };

    // One pylon, size 0x78 (nothing added to msw::CStgGmk_Pylon). Made by
    // msw::CStgGmk_PylonCluster_Dodge::NewGmk. All inline: the unit above
    // emits the vtable and NewAct, NewMdl and the destructor (reverse order
    // of the declarations here).
    class CStgGmk_Pylon_Dodge : public CStgGmk_Pylon {
        public:
            CStgGmk_Pylon_Dodge(bcs::CTask *pParent) : CStgGmk_Pylon(pParent, 0) {}
            virtual ~CStgGmk_Pylon_Dodge() {}

            virtual CStgMdl *NewMdl(long heap, unsigned long idx) { return new (heap) CStgGmk_Pylon_Mdl(this, 0); }
            virtual CStgAct *NewAct(long heap) { return new (heap) CStgGmk_Pylon_Act_Dodge(this); }
    };

    // The pylon cluster of the dodgeball mode, with the DODGE_PRM_S04_*
    // parameters. Size 0xA1C (nothing added to msw::CStgGmk_PylonCluster).
    // A member of msw::CStageDodge04, which inlines the constructor and
    // emits the weak destructor. The other functions follow in DodgeF
    // (game/dodge/CStgGmk_Pylon_Act_Dodge.cpp), with the vtable; NewGmk
    // and GetParam_LocPattern are inline (emitted there after
    // CStgGmk_PylonCluster's weak functions).
    class CStgGmk_PylonCluster_Dodge : public CStgGmk_PylonCluster {
        public:
            CStgGmk_PylonCluster_Dodge(bcs::CTask *pParent) : CStgGmk_PylonCluster(pParent) {}
            virtual ~CStgGmk_PylonCluster_Dodge() {}

            // DODGE_PRM_S04_PYLON_LOC_PAT_TYPE selects a kind of random
            // pattern (negated); zero means the fixed pattern
            // DODGE_PRM_S04_PYLON_LOC_PAT_NO.
            virtual long GetParam_LocPattern() const
            {
                if (DODGE_PRM_S04_PYLON_LOC_PAT_TYPE != 0) {
                    return -DODGE_PRM_S04_PYLON_LOC_PAT_TYPE;
                }
                return DODGE_PRM_S04_PYLON_LOC_PAT_NO;
            }
            // Declared after GetParam_LocPattern: the original emits it
            // before it (weak inlines come out in reverse declaration order).
            virtual CStgGmk *NewGmk(long heap) const { return new (heap) CStgGmk_Pylon_Dodge(GetParent()); }
            virtual long GetLocValidNumIdx() const;
            virtual long GetLocRawTblIdx(long pattern, long idx) const;
    };
}

#endif /* !MSW_CSTGGMK_PYLON_DODGE_HPP_ */
