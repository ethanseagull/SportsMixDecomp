#ifndef MSW_CSTAGEVOLLEY42_HPP_
#define MSW_CSTAGEVOLLEY42_HPP_

#include "game/msw/CStageManVolley.hpp"

namespace msw {

    // Volley stage 42 (made by CStageFactoryVolley). Adds no members to
    // CStageManVolley (size 0x200); it only sets up an orthographic
    // projection camera after the stage is initialised. Its
    // OnPostInitialize is the same code as CStageVolley10's.
    class CStageVolley42 : public CStageManVolley {
        public:
            CStageVolley42(bcs::CTask *pParent);
            virtual ~CStageVolley42();

            virtual void OnPostInitialize();
    };
}

#endif /* !MSW_CSTAGEVOLLEY42_HPP_ */
