#ifndef MSW_CMIIRENDERBASE_HPP_
#define MSW_CMIIRENDERBASE_HPP_

#include "bcs/CTask.hpp"
#include "revolution/rfl.h"

namespace msw {

    // A task that renders a Mii into a texture (MenuF). Its constructor also
    // builds a CChrMdlObj, a CAnmMotion and a CGenPrerendGrp, which are not
    // modelled here; only the members used so far. Argument names are
    // guesses.
    class CMiiRenderBase : public bcs::CTask {
        public:
            CMiiRenderBase(bcs::CTask *pParent, RFLStoreData &rData, long nWidth, long nHeight, unsigned long nUnk);
            virtual ~CMiiRenderBase();
            virtual void Main();
            virtual void MainFunc();

            // The rendered image, for GXInitTexObj.
            void *GetTexImg() const;
    };
}

#endif /* !MSW_CMIIRENDERBASE_HPP_ */
