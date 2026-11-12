#ifndef MSW_CSTGGMK_ICECUBE_HOCKEY_HPP_
#define MSW_CSTGGMK_ICECUBE_HOCKEY_HPP_

#include "bcs/New.hpp"
#include "game/msw/CAtkCollision.hpp"
#include "game/msw/CGame.hpp"
#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"
#include "game/msw/CStgGmk_Pylon_Hockey.hpp"
#include "game/msw/HockeyPrm.hpp"

// The ice cubes of hockey party stage 85 (HockeyF,
// game/hockey/CStgGmk_IceCube_Act_Hockey.cpp holds their functions and
// vtables). The classes are outside the msw namespace in the symbols. They
// are declared in the order that gives the original's weak functions (the
// inline functions come out in reverse order of these declarations).
//
// An ice cube is a pylon that slides with the tilt of the floor. Hit by a
// character's attack it is knocked straight along X or Z; while it moves
// fast enough it is "offensive": it hurts what it hits and is harder to
// push. Pushed off the field it falls into the water.

// The model of an ice cube ("s85_box_00"), size 0x130 (nothing added to
// msw::CStgGmk_Pylon_Mdl). Main hides the shadow below the floor and the
// reflection below the water.
class CStgGmk_IceCube_Mdl_Hockey : public msw::CStgGmk_Pylon_Mdl {
    public:
        CStgGmk_IceCube_Mdl_Hockey(msw::CStgGmk *pParent) : msw::CStgGmk_Pylon_Mdl(pParent, 0) {}
        virtual ~CStgGmk_IceCube_Mdl_Hockey();
        virtual void Create();
        virtual void Main(float rate);
        // The original names of these three slots are GetMdlWidth,
        // GetMdlDepth and GetMdlHeight (their symbols in this file);
        // msw::CStgGmk_Pylon_Mdl still declares them under guessed names.
        virtual float GetParam_OrgWidth() const;
        virtual float GetParam_OrgDepth() const;
        virtual float GetParam_OrgHeight() const;
};

// The actor of an ice cube, size 0x21C: an attack collision added to the
// hockey pylon's actor. IsOffensive and GetParam_AtkVel are new slots
// (0x2B8, 0x2BC).
class CStgGmk_IceCube_Act_Hockey : public msw::CStgGmk_Pylon_Act_Hockey {
    public:
        CStgGmk_IceCube_Act_Hockey(msw::CStgGmk *pParent);

        virtual float GetParam_MdlHeight() const { return msw::HOCKEY_PRM_S85_ICECUBE_MDL_HEIGHT; }
        virtual float GetParam_Height() const { return msw::HOCKEY_PRM_S85_ICECUBE_HEIGHT; }
        virtual float GetParam_Height2() const { return msw::HOCKEY_PRM_S85_ICECUBE_HEIGHT2; }
        // An offensive cube pushes harder and gives way less.
        virtual long GetParam_PushPower() const
        {
            return IsOffensive() ? msw::HOCKEY_PRM_S85_ICECUBE_PUSH_POWER_2 : msw::HOCKEY_PRM_S85_ICECUBE_PUSH_POWER;
        }
        virtual float GetParam_PushResist() const
        {
            if (IsOffensive()) {
                return msw::HOCKEY_PRM_S85_ICECUBE_PUSH_RESIST_2;
            }
            return msw::HOCKEY_PRM_S85_ICECUBE_PUSH_RESIST;
        }
        virtual float GetParam_Dec() const
        {
            return SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_DEC, msw::HOCKEY_PRM_S85_ICE1_DEC,
                                   msw::HOCKEY_PRM_S85_ICE2_DEC, msw::HOCKEY_PRM_S85_ICE3_DEC);
        }
        virtual float GetParam_VelLimit() const { return msw::HOCKEY_PRM_S85_ICECUBE_VEL; }
        // Given in seconds.
        virtual float GetParam_Interval() const
        {
            return 60.0f * SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_INTERVAL, msw::HOCKEY_PRM_S85_ICE1_INTERVAL,
                                           msw::HOCKEY_PRM_S85_ICE2_INTERVAL, msw::HOCKEY_PRM_S85_ICE3_INTERVAL);
        }
        virtual float GetParam_MaxLife() const { return 5.0f; }
        virtual float GetParam_FieldMargin() const { return 1.0f; }
        virtual bool GetParam_IsEnableAreaCheck() const { return false; }
        virtual float GetParam_LocRndX() const
        {
            return SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_LOC_RND_X, msw::HOCKEY_PRM_S85_ICE1_LOC_RND_X,
                                   msw::HOCKEY_PRM_S85_ICE2_LOC_RND_X, msw::HOCKEY_PRM_S85_ICE3_LOC_RND_X);
        }
        virtual float GetParam_LocRndZ() const
        {
            return SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_LOC_RND_Z, msw::HOCKEY_PRM_S85_ICE1_LOC_RND_Z,
                                   msw::HOCKEY_PRM_S85_ICE2_LOC_RND_Z, msw::HOCKEY_PRM_S85_ICE3_LOC_RND_Z);
        }
        virtual float GetParam_DragSpeed() const { return msw::HOCKEY_PRM_S85_ICECUBE_EFF_VEL; }
        // Declared after the getters: the original emits it before them.
        virtual ~CStgGmk_IceCube_Act_Hockey() {}

        virtual float GetParam_Radius() const;
        virtual float GetParam_MdlRadius() const;
        virtual float GetParam_Acc() const;
        virtual long GetParam_Effect(long type) const;
        virtual long GetParam_SE(long type) const;
        virtual void OnAttackHit(const msw::CCollAtkUnit *pAtk, const msw::CCollBodyUnit *pBody,
                                 const bcs::FVEC3 &rPos, unsigned long flag);
        virtual unsigned long OnBodyDamage(msw::CCollBodyUnit *pBody, const msw::CCollAtkUnit *pAtk,
                                           const bcs::FVEC3 &rPos);
        virtual unsigned long OnReflectHit(const msw::CCollDefUnit *pDef, const msw::CBall *pBall,
                                           bcs::FVEC3 &rPos, bcs::FVEC3 &rVel);
        virtual void UpdateColl();
        virtual int CheckCollBg(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                const bcs::FVEC3 &rTo);
        virtual void OnLanding();
        virtual void KickSplashEffect();
        virtual void CtorState_MOVE();
        virtual void LoopState_MOVE();
        virtual void DtorState_MOVE();
        virtual void CtorState_RESPAWN();
        virtual void LoopState_RESPAWN();
        virtual void DtorState_RESPAWN();

        // Whether the cube moves at GetParam_AtkVel or faster.
        virtual bool IsOffensive() const;  // 0x2B8
        virtual float GetParam_AtkVel() const; // 0x2BC

    private:
        // The value of the party game's pattern (0 to 3).
        static float SelectByPattern(const float &r0, const float &r1, const float &r2, const float &r3)
        {
            if (msw::CGame::GetPartyGameParam(0) == 0) {
                return r0;
            } else if (msw::CGame::GetPartyGameParam(0) == 1) {
                return r1;
            } else if (msw::CGame::GetPartyGameParam(0) == 2) {
                return r2;
            } else {
                return r3;
            }
        }

        msw::CAtkCollision m_AtkColl; // 0x210
};

// An ice cube: a hockey pylon (msw::CStgGmk_Pylon_Hockey) with the ice
// cube's actor and model.
// Size 0x78 (nothing added).
class CStgGmk_IceCube_Hockey : public msw::CStgGmk_Pylon_Hockey {
    public:
        CStgGmk_IceCube_Hockey(bcs::CTask *pParent) : msw::CStgGmk_Pylon_Hockey(pParent) {}
        virtual ~CStgGmk_IceCube_Hockey() {}
        virtual msw::CStgMdl *NewMdl(long heap, unsigned long idx)
        {
            return new (heap) CStgGmk_IceCube_Mdl_Hockey(this);
        }
        virtual msw::CStgAct *NewAct(long heap) { return new (heap) CStgGmk_IceCube_Act_Hockey(this); }
        virtual void SetAccel(const bcs::FVEC3 &rAccel);
        virtual bool IsOffensive() const;
};

// The ice cubes of the stage, set out after the HOCKEY_PRM_S85_ICE*
// location patterns. Size 0xA1C (nothing added to the hockey pylon cluster).
// CStageHockeyCrushSkate85 has one as a member, inlines the constructor and
// the destructor.
class CStgGmk_IceCubeCluster_Hockey : public msw::CStgGmk_PylonCluster_Hockey {
    public:
        CStgGmk_IceCubeCluster_Hockey(bcs::CTask *pParent) : msw::CStgGmk_PylonCluster_Hockey(pParent) {}
        virtual ~CStgGmk_IceCubeCluster_Hockey() {}

        // The party game's pattern selects the location pattern: a kind
        // of random pattern (negated), or the fixed pattern ICE*_LOC_PAT_NO
        // when the kind is zero.
        virtual long GetParam_LocPattern() const
        {
            long type = SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_LOC_PAT_TYPE, msw::HOCKEY_PRM_S85_ICE1_LOC_PAT_TYPE,
                                        msw::HOCKEY_PRM_S85_ICE2_LOC_PAT_TYPE, msw::HOCKEY_PRM_S85_ICE3_LOC_PAT_TYPE);
            if (type != 0) {
                return -type;
            }
            return SelectByPattern(msw::HOCKEY_PRM_S85_ICE0_LOC_PAT_NO, msw::HOCKEY_PRM_S85_ICE1_LOC_PAT_NO,
                                   msw::HOCKEY_PRM_S85_ICE2_LOC_PAT_NO, msw::HOCKEY_PRM_S85_ICE3_LOC_PAT_NO);
        }
        virtual msw::CStgGmk *NewGmk(long heap) const { return new (heap) CStgGmk_IceCube_Hockey(GetParent()); }
        virtual long GetLocValidNumIdx() const;
        virtual long GetLocRawTblIdx(long pattern, long idx) const;

        // (name guessed) Ice cube idx, through GetGmk_.
        CStgGmk_IceCube_Hockey *GetIceCube(long idx)
        {
            return static_cast<CStgGmk_IceCube_Hockey *>(GetGmk_(idx));
        }
        const CStgGmk_IceCube_Hockey *GetIceCube(long idx) const
        {
            return static_cast<const CStgGmk_IceCube_Hockey *>(GetGmk_(idx));
        }

    private:
        // The value of the party game's pattern (0 to 3).
        static long SelectByPattern(const long &r0, const long &r1, const long &r2, const long &r3)
        {
            if (msw::CGame::GetPartyGameParam(0) == 0) {
                return r0;
            } else if (msw::CGame::GetPartyGameParam(0) == 1) {
                return r1;
            } else if (msw::CGame::GetPartyGameParam(0) == 2) {
                return r2;
            } else {
                return r3;
            }
        }
};

#endif /* !MSW_CSTGGMK_ICECUBE_HOCKEY_HPP_ */
