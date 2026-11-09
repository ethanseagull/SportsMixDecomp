#ifndef BCS_G2D_CPANEOBJ_HPP_
#define BCS_G2D_CPANEOBJ_HPP_

namespace bcs {
    namespace G2D {

        // Lives in the DOL. Only the member used so far; the meaning of the
        // arguments after the animation number is not known.
        class CPaneObj {
            public:
                void PlayAnimation(long nAnim, float fSpeed, float fFrame, int nArg0, int nArg1, int nArg2);
        };
    }
}

#endif /* !BCS_G2D_CPANEOBJ_HPP_ */
