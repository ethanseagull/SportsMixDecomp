#ifndef MSW_CCOMDRVVOLLEYGAME_HPP_
#define MSW_CCOMDRVVOLLEYGAME_HPP_

#include "game/msw/CComDrvVolley.hpp"

namespace msw {

    // INCOMPLETE: the computer driver of the normal volleyball game
    // (VolleyF), size 0x1FC (what CChrManVolley::CreateDriver_ allocates).
    // Only the constructor is declared.
    class CComDrvVolleyGame : public CComDrvVolley {
        public:
            CComDrvVolleyGame();

        protected:
            unsigned char m_Unk170[0x1FC - 0x170];
    };
}

#endif /* !MSW_CCOMDRVVOLLEYGAME_HPP_ */
