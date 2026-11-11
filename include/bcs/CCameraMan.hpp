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
            static void RemoveEntryAll();

        private:
            enum {
                VIEW_MAX = 4,    // (name guessed)
                ENTRY_MAX = 16   // (name guessed)
            };

            // (name guessed) The cameras entered for each view; the first
            // one is the active camera. The first object of the DOL's .bss
            // (0x804BF580); bcs::CCamera's functions read it too.
            static CCamera *m_pCamera[VIEW_MAX][ENTRY_MAX];
    };
}

#endif /* !BCS_CCAMERAMAN_HPP_ */
