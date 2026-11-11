#ifndef BCS_CCAMERAMAN_HPP_
#define BCS_CCAMERAMAN_HPP_

namespace bcs {

    class CCamera;

    // The cameras of the views (DOL). Only the members used so far; both
    // take no object.
    class CCameraMan {
        public:
            // The camera active in view nView, 0 for none.
            static CCamera *GetActiveCamera(long nView);
            // Non-zero while the debug camera runs in view nView.
            static int IsDbgCamera(long nView);
    };
}

#endif /* !BCS_CCAMERAMAN_HPP_ */
