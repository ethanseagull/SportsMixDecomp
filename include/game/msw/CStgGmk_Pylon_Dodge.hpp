#ifndef MSW_CSTGGMK_PYLON_DODGE_HPP_
#define MSW_CSTGGMK_PYLON_DODGE_HPP_

#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"

namespace msw {

    // The pylon cluster of the dodgeball mode, with the DODGE_PRM_S04_*
    // parameters. Size 0xA1C (nothing added to msw::CStgGmk_PylonCluster).
    // A member of msw::CStageDodge04, which inlines the constructor and
    // emits the weak destructor. The other functions follow in DodgeF
    // (game/dodge/CStgGmk_Pylon_Act_Dodge.cpp), with the vtable.
    class CStgGmk_PylonCluster_Dodge : public CStgGmk_PylonCluster {
        public:
            CStgGmk_PylonCluster_Dodge(bcs::CTask *pParent) : CStgGmk_PylonCluster(pParent) {}
            virtual ~CStgGmk_PylonCluster_Dodge() {}

            virtual CStgGmk *NewGmk(long heap) const;
            virtual long GetParam_LocPattern() const;
            virtual long GetLocValidNumIdx() const;
            virtual long GetLocRawTblIdx(long, long) const;
    };
}

#endif /* !MSW_CSTGGMK_PYLON_DODGE_HPP_ */
