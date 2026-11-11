#ifndef MSW_CMIIRENDERBASE_HPP_
#define MSW_CMIIRENDERBASE_HPP_

#include "bcs/CTask.hpp"
#include "revolution/rfl.h"

namespace bcs {
    class CAnmMotion;
}

namespace msw {

    class CChrMdlObj;
    class CGenPrerendGrp;

    // A task that renders a Mii into a texture (MenuF). Size 0x48 (the
    // derived classes without members of their own are allocated so). It
    // builds the Mii's model and its motion and draws them through a
    // CGenPrerendGrp of nWidth x nHeight. Defined in
    // game/menu/CTopMenuChrObj.cpp; names of the members and arguments are
    // guesses.
    class CMiiRenderBase : public bcs::CTask {
        public:
            // nFlag is passed on to CChrMdlObj::CreatePlayerInfoObj.
            CMiiRenderBase(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight, unsigned long nFlag);
            virtual ~CMiiRenderBase();
            // Runs MainFunc, then advances and places the model.
            virtual void Main();
            virtual void MainFunc();

            // The rendered image, for GXInitTexObj.
            void *GetTexImg() const;

        protected:
            CChrMdlObj *m_pMdl;          // 0x34
            bcs::CAnmMotion *m_pMotion;  // 0x38
            CGenPrerendGrp *m_pPrerend;  // 0x3C
            long m_nPrerendMode;         // 0x40, copied to the CGenPrerendGrp every frame (meaning not known)
            long m_Unk44;                // 0x44, 0 (never read here)
    };
}

#endif /* !MSW_CMIIRENDERBASE_HPP_ */
