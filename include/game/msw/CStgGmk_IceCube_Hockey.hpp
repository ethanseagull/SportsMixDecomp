#ifndef MSW_CSTGGMK_ICECUBE_HOCKEY_HPP_
#define MSW_CSTGGMK_ICECUBE_HOCKEY_HPP_

#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"

// INCOMPLETE: the ice cubes of hockey party stage 85 (HockeyF,
// game/hockey/CStgGmk_IceCube_Act_Hockey.cpp holds their functions and
// vtables). Both classes are outside the msw namespace in the symbols.
// Declared as far as CStageHockeyCrushSkate85 needs them; the overrides are
// those of __vt__22CStgGmk_IceCube_Hockey and
// __vt__29CStgGmk_IceCubeCluster_Hockey.

// An ice cube: a msw::CStgGmk_Pylon that slides with the tilt of the floor.
class CStgGmk_IceCube_Hockey : public msw::CStgGmk_Pylon {
    public:
        virtual ~CStgGmk_IceCube_Hockey();
        virtual msw::CStgAct *NewAct(long heap);
        virtual msw::CStgMdl *NewMdl(long heap, unsigned long);
        virtual void SetAccel(const bcs::FVEC3 &rAccel);
        virtual bool IsOffensive() const;
};

// The ice cubes of the stage, set out after the HOCKEY_PRM_S85_ICE*
// location patterns. Size 0xA1C (nothing added to the pylon cluster).
// CStageHockeyCrushSkate85 has one as a member, inlines the constructor and
// the destructor.
class CStgGmk_IceCubeCluster_Hockey : public msw::CStgGmk_PylonCluster {
    public:
        CStgGmk_IceCubeCluster_Hockey(bcs::CTask *pParent) : msw::CStgGmk_PylonCluster(pParent) {}

        virtual msw::CStgGmk *NewGmk(long heap) const;
        virtual long GetParam_LocPattern() const;
        virtual long GetLocValidNumIdx() const;
        virtual long GetLocRawTblIdx(long, long) const;

        // (name guessed) Ice cube idx, through GetGmk_.
        CStgGmk_IceCube_Hockey *GetIceCube(long idx)
        {
            return static_cast<CStgGmk_IceCube_Hockey *>(GetGmk_(idx));
        }
        const CStgGmk_IceCube_Hockey *GetIceCube(long idx) const
        {
            return static_cast<const CStgGmk_IceCube_Hockey *>(GetGmk_(idx));
        }
};

#endif /* !MSW_CSTGGMK_ICECUBE_HOCKEY_HPP_ */
