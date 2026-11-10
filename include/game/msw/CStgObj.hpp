#ifndef MSW_CSTGOBJ_HPP_
#define MSW_CSTGOBJ_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    // A stage object (gimmicks, the volley net). A DOL class derived from
    // bcs::CUnivTask. Only the virtual functions up to SetPos are declared,
    // in the order of __vt__Q23msw15CStageNetVolley (VolleyF); more follow
    // (Main is not declared: whether CStgObj has one is not known). Return
    // types are guessed. The members from 0x34 are not mapped here.
    class CStgObj : public bcs::CUnivTask {
        public:
            virtual void SetIdx(long idx);      // 0x18
            virtual long GetIdx() const;        // 0x1C
            virtual void Create();              // 0x20
            virtual void SceneEntry();          // 0x24
            virtual void SceneLeave();          // 0x28
            virtual void *GetSrt();             // 0x2C (return type not known)
            virtual const void *GetSrt() const; // 0x30 (return type not known)
            virtual void SetPos(const bcs::FVEC3 &rPos); // 0x34
    };
}

#endif /* !MSW_CSTGOBJ_HPP_ */
