#ifndef MSW_CSTGGMK_PYLON_HOCKEY_HPP_
#define MSW_CSTGGMK_PYLON_HOCKEY_HPP_

#include "game/msw/CStgGmk_Pylon.hpp"
#include "game/msw/CStgGmk_PylonCluster.hpp"

namespace msw {

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
