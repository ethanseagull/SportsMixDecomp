#ifndef MSW_CITEMBALLSHELLWRECK_HPP_
#define MSW_CITEMBALLSHELLWRECK_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CBall;

    // The shell of a shell item ball flying off when the ball breaks it: the
    // shell model is thrown back from the ball's direction of travel, falls,
    // tips over and fades out over its last 30 frames, then the task deletes
    // itself. A DOL class, size 0x5C, vtable lbl_8046B6E8 (destructor and
    // Main have no exported names). Member names are guessed.
    class CItemBallShellWreck : public bcs::CUnivTaskEx {
        public:
            // nItemKind selects the model (msw::CItem::GetItemRsrcFile).
            CItemBallShellWreck(CBall *pBall, long nItemKind);
            virtual ~CItemBallShellWreck();
            virtual void Main();

        private:
            long m_nStep;          // 0x34
            float m_fTimer;        // 0x38, frames left (30 at the start)
            bcs::FVEC3 m_vPos;     // 0x3C
            bcs::FVEC3 m_vVel;     // 0x48
            float m_fGravity;      // 0x54
            bcs::CMdlObj *m_pMdl;  // 0x58
    };
}

#endif /* !MSW_CITEMBALLSHELLWRECK_HPP_ */
