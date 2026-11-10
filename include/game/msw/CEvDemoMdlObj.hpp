#ifndef CEVDEMOMDLOBJ_HPP_
#define CEVDEMOMDLOBJ_HPP_

#include "game/msw/CEventModel.hpp"

namespace msw {

    // An object model of an event demo. A DOL class built on
    // msw::CEventModel. Only the members used so far.
    class CEvDemoMdlObj : public CEventModel {
        public:
            // Size 0x14. What CreateModel builds a model from. The layout
            // is read from the two callers in TourF
            // (msw::CEvMdlCreaterTmMdlObj and CEvMdlCreaterTmMdlNpc) and
            // from the DOL's CreateModel, which fills pInit, both files and
            // nType itself. The constructor is inline there; its parameter
            // and the member names are guesses.
            struct _SETUP {
                _SETUP(int disp)
                {
                    pMdlRes = 0;
                    pMotRes = 0;
                    pInit = 0;
                    nType = 0;
                    nIndex = 0;
                    nDrawLayer = 2;
                    bDisp = disp;
                    nShadowSize = -1;
                    nLightSelector = -1;
                }

                const void *pInit;           // 0x00
                nw4r::g3d::ResFile *pMdlRes; // 0x04: file with the model
                nw4r::g3d::ResFile *pMotRes; // 0x08: file with its animations
                short nType;                 // 0x0C: CEventModel::GetType
                signed char nIndex;          // 0x0E: CEventModel::GetIndex
                signed char nDrawLayer;      // 0x0F
                signed char bDisp;           // 0x10
                signed char nLightSelector;  // 0x11: -1 for none
                short nShadowSize;           // 0x12: -1 for none
            };

            // Static: their bodies in the DOL take the setup in r3. The
            // second one looks up model `pName` in archive `nArc` (a
            // msw::CLoadData entry, through EvDemo::GetArcResDataMdl), uses
            // that file for the animations too, and stores pInit and the
            // archive entry's number in the setup before making the model.
            // (meaning of the int not known; the callers pass 0)
            static CEvDemoMdlObj *CreateModel(const _SETUP &rSetup, int);
            static CEvDemoMdlObj *CreateModel(_SETUP &rSetup, long nArc, const char *pName, const char *pInit);
    };
}

#endif /* !CEVDEMOMDLOBJ_HPP_ */
