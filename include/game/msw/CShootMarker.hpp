#ifndef MSW_CSHOOTMARKER_HPP_
#define MSW_CSHOOTMARKER_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    // INCOMPLETE: the marker of where a character shoots (HockeyF
    // game/hockey/CShootMarker.cpp), size 0x60, one per team. A task.
    class CShootMarker : public bcs::CTask {
        public:
            CShootMarker(bcs::CTask *pParent, long nTeam);
            virtual ~CShootMarker();
            virtual void Main();

            // (name guessed) Set to 1 by CChrManHockey::ResetShootMarker.
            void SetReset() { m_bReset = 1; }

        protected:
            unsigned char m_Unk34[0x58 - 0x34];
            long m_bReset; // 0x58 (name guessed)
            unsigned char m_Unk5C[0x60 - 0x5C];
    };
}

#endif /* !MSW_CSHOOTMARKER_HPP_ */
