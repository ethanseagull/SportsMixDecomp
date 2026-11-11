#ifndef MSW_CTOPMENUCHR_HPP_
#define MSW_CTOPMENUCHR_HPP_

#include "bcs/CTask.hpp"

namespace bcs {
    class CScnObj;
}

namespace nw4r {
    namespace math {
        struct AABB;
    }
}

namespace msw {

    class CTopMenuChrObj;
    class CBBoxObj;

    // The characters of the sports shown on the top menu (MenuF). Size 0x64
    // (layout.py). Defined in game/menu/CTopMenuChrObj.cpp. Names after the
    // mangled names; return and argument meanings are guesses.
    class CTopMenuChr : public bcs::CTask {
        public:
            CTopMenuChr(bcs::CTask *pParent);
            virtual ~CTopMenuChr();
            virtual void Main();
            virtual void Disp();

            void EnableDisp(int bDisp);
            void EnableMixDisp(int bDisp);
            // The gold model of character nChr (0 to 3).
            void EnableGoldModel(long nChr, int bGold);
            // Hangs character nChr on node nNode of pParent.
            void AttachTo(long nChr, bcs::CScnObj *pParent, long nNode);
            // Turns to the character of game nGame (-1: none) in fFrame frames.
            void SetSelectGame(long nGame, float fFrame);
            // Not zero when a character holds its decide motion.
            int PrepareDecideAnim();
            int IsReadyDecideAnim() const;
            void PlayDecideAnim(float fFrame);
            int IsEndDecideAnim() const;
            void Reset();
            void SetMotionSpeedRate(float fRate);
            // The screen box of character nChr; 1 when it is valid.
            long GetBBoxInfo(long nChr, nw4r::math::AABB &rBox);

            // The number of characters: the four sports and the mixed
            // games.
            enum {
                CHR_NUM = 5
            };

        private:
            CTopMenuChrObj *m_pChrObj[CHR_NUM]; // 0x34
            // 0x48, the screen box of each character, attached to its model
            CBBoxObj *m_pBBox[CHR_NUM];
            long m_nSelectGame; // 0x5C, -1 for none
            int m_bMixDisp;     // 0x60, the mixed games' character is shown
    };
}

#endif /* !MSW_CTOPMENUCHR_HPP_ */
