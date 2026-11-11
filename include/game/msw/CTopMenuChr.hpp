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

    // The characters of the sports shown on the top menu (MenuF). Size 0x64
    // (layout.py). Only the members msw::SelectMenu::CBgChr uses; their
    // bodies are in another MenuF unit. Names after the mangled names;
    // return and argument meanings are guesses.
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
            void PrepareDecideAnim();
            int IsReadyDecideAnim() const;
            void PlayDecideAnim(float fFrame);
            int IsEndDecideAnim() const;
            void Reset();
            void SetMotionSpeedRate(float fRate);
            // The screen box of character nChr; 1 when it is valid.
            long GetBBoxInfo(long nChr, nw4r::math::AABB &rBox);

        private:
            unsigned char m_Unk34[0x30]; // 0x34
    };
}

#endif /* !MSW_CTOPMENUCHR_HPP_ */
