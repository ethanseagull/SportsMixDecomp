#ifndef CEVDEMOMDLCHR_HPP_
#define CEVDEMOMDLCHR_HPP_

#include "game/msw/CEventModel.hpp"

namespace msw {

    // A character model of an event demo. A DOL class, 0xE0 bytes (the size
    // CreateModel allocates), built on msw::CEventModel; its own members
    // (from 0xD0) are not mapped. Only the members used so far.
    class CEvDemoMdlChr : public CEventModel {
        public:
            // Size 0x18. What CreateModel builds a model from. The layout
            // is read from CreateModel in the DOL and from the two callers
            // in the modules (msw::CEvMdlCreaterTmMdlChr in TourF,
            // msw::CEvMdlCreaterGmMdlChr in BossF), which both fill it the
            // same way; the constructor is inline there and its parameters
            // are a guess, as are the member names.
            struct _SETUP {
                _SETUP(nw4r::g3d::ResFile &rMdlRes, nw4r::g3d::ResFile &rMotRes, int disp)
                {
                    pMotTable = 0;
                    pInit = 0;
                    nType = 0;
                    nIndex = 0;
                    nDrawLayer = 2;
                    bDisp = disp;
                    nUnk16 = -1;
                    nLightSelector = 0;
                    pMdlRes = &rMdlRes;
                    pMotRes = &rMotRes;
                }

                void *pInit;                 // 0x00: kept by the model at 0xD0
                const short *pMotTable;      // 0x04: the model's motion table
                nw4r::g3d::ResFile *pMdlRes; // 0x08: file with the model
                nw4r::g3d::ResFile *pMotRes; // 0x0C: file with its animations
                short nType;                 // 0x10: CEventModel::GetType; the character
                signed char nIndex;          // 0x12: CEventModel::GetIndex
                signed char nDrawLayer;      // 0x13
                signed char bDisp;           // 0x14: argument of SetDisp
                signed char nLightSelector;  // 0x15: argument of SetLightSelector; -1 for none
                short nUnk16;                // 0x16
            };

            // Static: its body in the DOL takes the setup in r3. (meaning
            // of the second argument, kept by the model at 0xDC, not known)
            static CEvDemoMdlChr *CreateModel(const _SETUP &rSetup, long);
    };
}

#endif /* !CEVDEMOMDLCHR_HPP_ */
