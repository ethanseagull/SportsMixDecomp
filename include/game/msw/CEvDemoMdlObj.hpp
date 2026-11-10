#ifndef CEVDEMOMDLOBJ_HPP_
#define CEVDEMOMDLOBJ_HPP_

#include "game/msw/CEventModel.hpp"

namespace msw {

    // An object model of an event demo (the props and the NPCs). A DOL
    // class built on msw::CEventModel; its own members are not mapped.
    // Only the members used so far.
    class CEvDemoMdlObj : public CEventModel {
        public:
            // Size 0x14. What CreateModel builds a model from. The layout is
            // read from the two creaters in BossF (msw::CEvMdlCreaterGmMdlObj
            // and GmMdlNpc), which fill it the same way; the constructor is
            // inline there and its parameter and the member names are
            // guesses.
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

                void *pInit;                 // 0x00
                nw4r::g3d::ResFile *pMdlRes; // 0x04: file with the model
                nw4r::g3d::ResFile *pMotRes; // 0x08: file with its animations
                short nType;                 // 0x0C
                signed char nIndex;          // 0x0E
                signed char nDrawLayer;      // 0x0F
                signed char bDisp;           // 0x10
                signed char nLightSelector;  // 0x11
                short nShadowSize;           // 0x12
            };

            // Static: their bodies in the DOL take the setup in r3. The
            // first makes the model from the given files; the second from
            // model pName in archive nArc (a msw::CLoadData entry; pSub as
            // for EvDemo::InitArcResData). (meaning of the int not known;
            // parameter names guessed)
            static CEvDemoMdlObj *CreateModel(const _SETUP &rSetup, int);
            static CEvDemoMdlObj *CreateModel(_SETUP &rSetup, long nArc, const char *pSub, const char *pName);
            // (parameter names guessed) A model from file pFile of the
            // effect resources (msw::CTournamentMan, TourF).
            static CEvDemoMdlObj *CreateModelEffRes(_SETUP &rSetup, const char *pFile, const char *pName);
    };
}

#endif /* !CEVDEMOMDLOBJ_HPP_ */
