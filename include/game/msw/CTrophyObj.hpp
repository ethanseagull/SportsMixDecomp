#ifndef CTROPHYOBJ_HPP_
#define CTROPHYOBJ_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CScnGroup;
}

namespace msw {

    // A cup model of the tournament. A DOL class, 0x1D0 bytes (the size
    // CreateTrophy allocates), built on bcs::CMdlObj (msw::CTournamentTrophy
    // in TourF calls CMdlObj's functions on it and hands it to
    // bcs::CScnObj::AppendDelayDeleteObject). Only the members used so far;
    // which are static is read from the callers (the ones taking the model
    // as their first argument are static), return types are guessed.
    class CTrophyObj : public bcs::CMdlObj {
        public:
            // Makes the cup of cup `nCup` from the model in rFile.
            // (meaning of the last argument not known)
            static CTrophyObj *CreateTrophy(nw4r::g3d::ResFile file, long nCup, long nGameMode, int);
            // Adds the model inside the cup.
            static void CreateTrophyIn(CTrophyObj *pTrophy, nw4r::g3d::ResFile file, long nCup, long nGameMode);
            static void SetCrystalColor(bcs::CMdlObj *pMdl, nw4r::g3d::ResFile file, long nGameMode, long nCup);
            // pName names the texture; nArc is the msw::CLoadData entry the
            // cup was loaded from.
            static void SetCrystalEnvTex(bcs::CMdlObj *pMdl, const char *pName, long nArc, long nStageGameMode, long nCup);

            void Destroy();
            void SetDisp(int bDisp);
            void _SetDrawPriority(long nPriority);
            void _SetLightSelector(long nSelector);
            void _ResetLightSelector();
            void _SetBaseColor(bcs::BCS_Color color);
            void _SetColorScale(bcs::BCS_Color color);
            // Puts the cup into a group, or into draw layer nLayer.
            void _PushBack(bcs::CScnGroup *pGroup);
            void _PushBack(long nLayer);
    };
}

#endif /* !CTROPHYOBJ_HPP_ */
