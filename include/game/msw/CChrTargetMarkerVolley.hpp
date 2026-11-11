#ifndef MSW_CCHRTARGETMARKERVOLLEY_HPP_
#define MSW_CCHRTARGETMARKERVOLLEY_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // INCOMPLETE: the target marker of a volleyball character (VolleyF), a
    // task of size 0xD8 (what CChrManVolley::CreateMarker_ allocates). Only
    // the constructor is declared.
    class CChrTargetMarkerVolley : public bcs::CTask {
        public:
            // (parameter names guessed) CChrManVolley passes itself, the
            // character's index and whether a player drives it.
            CChrTargetMarkerVolley(bcs::CTask *pParent, long nChr, int bPlayer);
            virtual ~CChrTargetMarkerVolley();
            virtual void Main();

        protected:
            unsigned char m_Unk34[0xD8 - 0x34];
    };
}

#endif /* !MSW_CCHRTARGETMARKERVOLLEY_HPP_ */
