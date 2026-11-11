#ifndef MSW_CCOMMANVOLLEYGAME_HPP_
#define MSW_CCOMMANVOLLEYGAME_HPP_

#include "game/msw/CComMan.hpp"

namespace msw {

    // The computer players' manager of the volleyball game (VolleyF). Size
    // 4: the vptr alone. CreateInstance makes the one instance. (It derives
    // from CComMan directly: with CComManVolley between them that class's
    // vtable would be emitted here, and the original has none.)
    class CComManVolleyGame : public CComMan {
        public:
            virtual ~CComManVolleyGame();
            virtual void Clear();
            virtual void Main(float fFrame);

            static void CreateInstance();
            // (meaning guessed) How far a computer character keeps its
            // handicap distance: the parameter for its level, more in
            // state 6, never negative when its team has a human player.
            static float GetConditionHandicapDis(const CChrCore *pChr);
    };
}

#endif /* !MSW_CCOMMANVOLLEYGAME_HPP_ */
