#ifndef MSW_CSTGGENERICGMKMDL_HPP_
#define MSW_CSTGGENERICGMKMDL_HPP_

#include "game/msw/CStgMdl.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    // (minimal declaration, added for VolleyF
    // game/volley/CStgGmk_WaterWorks_Volley_Act.cpp) A gimmick model made
    // from one MDL_PARAM entry of a resource file. A DOL class, size 0x140
    // (the allocation in msw::CStgGmk_WaterWorks_Volley::NewMdl); constructor
    // __ct__Q23msw17CStgGenericGmkMdlFPQ23msw7CStgObjQ34nw4r3g3d7ResFilePCQ33msw7CStgMdl9MDL_PARAMPCc.
    // The meaning of the last argument is not known (null there).
    class CStgGenericGmkMdl : public CStgMdl {
        public:
            CStgGenericGmkMdl(CStgObj *pParent, nw4r::g3d::ResFile res, const MDL_PARAM *pParam, const char *pName);
            virtual ~CStgGenericGmkMdl();

        private:
            unsigned char m_Unk12C[0x14]; // 0x12C (not mapped), size 0x140
    };
}

#endif /* !MSW_CSTGGENERICGMKMDL_HPP_ */
