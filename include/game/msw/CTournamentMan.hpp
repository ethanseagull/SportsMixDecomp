#ifndef CTOURNAMENTMAN_HPP_
#define CTOURNAMENTMAN_HPP_

#include "bcs/FVEC3.hpp"

namespace msw {

    class CEventModel;

    // The tournament map's manager (module TourF). Only the members used so
    // far.
    class CTournamentMan {
        public:
            static CTournamentMan *GetInstance();

            // Deletes the two objects the manager keeps for character
            // `nIndex`; with bDestroy set it also has msw::CEvDemoRes
            // destroy the character's model. (parameter names guessed)
            void CleanChrModel(long nIndex, int bDestroy);
            // The same for object `nIndex`. (parameter names guessed)
            void CleanObjModel(long nIndex, int bDestroy);

            // Creates map object `nIndex` from model `pName` (variant nSub)
            // unless it exists already, puts it at vPos and returns it.
            // The other parameters go to msw::CEvDemoRes's display setup;
            // pAnim (may be 0) is played on it and nNode (-1 for none) set.
            // Ex is the variant the stage's main model (the cup) uses.
            // (names and return type guessed)
            CEventModel *SetupObjModel(long nIndex, long nSub, char *pName, long nDisp, const bcs::FVEC3 &vPos,
                                       long nLayer, long nGroup, int b, const char *pAnim, long nNode);
            CEventModel *SetupObjModelEx(long nIndex, long nSub, char *pName, long nDisp, const bcs::FVEC3 &vPos,
                                         long nLayer, long nGroup, int b, const char *pAnim, long nNode);

            // Map sounds, kept in the slots nFirst..nLast. (names and
            // return types guessed)
            long GetSeSlot(long nFirst, long nLast);
            void PlaySe(long nSe, const bcs::FVEC3 &vPos, long nSlot, float fParam1, float fVolume, float fParam3,
                        unsigned long ulFlag);
            void StopSe(float fFade, long nFirst, long nLast);
            int IsPlaySe(long nSlot);
    };
}

#endif /* !CTOURNAMENTMAN_HPP_ */
