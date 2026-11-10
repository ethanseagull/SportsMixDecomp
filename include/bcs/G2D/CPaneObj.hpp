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

                nw4r::lyt::Pane *GetPane() const { return m_pPane; }

            private:
                unsigned char m_Unk00[8];
                nw4r::lyt::Pane *m_pPane; // 0x08
        };
    }
}

#endif /* !BCS_G2D_CPANEOBJ_HPP_ */
