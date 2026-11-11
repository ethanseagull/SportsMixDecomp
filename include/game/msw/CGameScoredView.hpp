#ifndef CGAMESCOREDVIEW_HPP_
#define CGAMESCOREDVIEW_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CCamera;
}

namespace msw {

    // The close camera on the character who scored (DOL,
    // game/msw/CGameExDemoBase.cpp; 0x3C bytes, vtable lbl_8048D408).
    // msw::CGame::StartScoredView_ makes it and calls Start, EndScoredView_
    // calls End. The class name is taken from those functions; the member
    // names are guesses.
    class CGameScoredView : public bcs::CTask {
        public:
            CGameScoredView(bcs::CTask *pParent);
            virtual ~CGameScoredView();
            virtual void Main();

            void Start(long nTeam);
            void End();

        private:
            // Hides the characters between the camera at rCamPos and the
            // character nLookChr, and turns down the others' sound.
            void SetChrHidden_(long nLookChr, const bcs::FVEC3 &rCamPos);

            long m_nLookChr;         // 0x34
            bcs::CCamera *m_pCamera; // 0x38
    };
}

#endif /* !CGAMESCOREDVIEW_HPP_ */
