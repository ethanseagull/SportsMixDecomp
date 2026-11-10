#ifndef BCS_G2D_CPANECTRL_HPP_
#define BCS_G2D_CPANECTRL_HPP_

#include "bcs/G2D/CLayoutBase.hpp"

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace bcs {
    namespace G2D {
        class CPaneObj;

        // Lives in the DOL. Size 0x54 (the size passed to operator new in
        // msw::CSelectMenuLayoutBase::CreateLytObj). CLayoutBase is at 0:
        // the same pointer is passed to functions taking either type.
        // Only the members used so far.
        class CPaneCtrl : public CLayoutBase {
            public:
                // The names of the two ints are guesses.
                void PlayAnim(long nAnim, float fSpeed, float fFrame, int bRecursive, int bReset);

                // Added for TourF game/tour/CCockpitTournamentMenuWinTrophy.cpp,
                // which creates one with `new (0)`.
                CPaneCtrl();

                // Return types are guesses except where the caller uses the
                // result.
                // The object of the pane called pName, or null.
                CPaneObj *GetChildPaneObj(const char *pName);
                // Makes the object that drives pPane; null on failure.
                CPaneObj *CreatePaneObj(nw4r::lyt::Pane *pPane);
                // Hangs pPaneObj's pane under rParent's.
                void AppendPaneObj(CPaneObj *pPaneObj, CPaneObj &rParent);

                // Added for MenuF game/menu/CSelectMenuLayoutBase.cpp.
                void SetVisible(int bVisible);

                // Added for MenuF game/menu/CButton2.cpp. The argument name
                // and the return type are guesses.
                void SetPaneZSort(int bZSort);

            private:
                // Brings the size to the 0x54 that `new` is asked for. The
                // constructor sets up a nw4r::ut::List here.
                unsigned char m_Unk48[0xC]; // 0x48
        };
    }
}

#endif /* !BCS_G2D_CPANECTRL_HPP_ */
