#ifndef BCS_G2D_CPANEOBJ_HPP_
#define BCS_G2D_CPANEOBJ_HPP_

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the member used so far; the meaning of the
        // arguments after the animation number is not known.
        class CPaneObj {
            public:
                void PlayAnimation(long nAnim, float fSpeed, float fFrame, int nArg0, int nArg1, int nArg2);
                // Added for HockeyF game/hockey/CCockpitHockeyCoin.cpp: the
                // animation to play when the current one ends (return type
                // guessed).
                void PushNextAnimation(long nAnim, float fSpeed, float fFrame, int nArg0, int nArg1);
                // Added for BasketF game/basket/CCockpitBasketPauseResult.cpp
                // (return types guessed): stops animation nAnim; not zero
                // once animation nAnim has ended.
                void StopAnimation(long nAnim, int nArg);
                int IsAnimEnd(long nAnim);

                nw4r::lyt::Pane *GetPane() const { return m_pPane; }

            private:
                unsigned char m_Unk00[8];
                nw4r::lyt::Pane *m_pPane; // 0x08
        };
    }
}

#endif /* !BCS_G2D_CPANEOBJ_HPP_ */
