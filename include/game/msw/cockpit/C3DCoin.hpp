#ifndef COCKPIT_C3DCOIN_HPP_
#define COCKPIT_C3DCOIN_HPP_

namespace bcs {
    class CTask;
}

namespace nw4r {
    namespace lyt {
        class Pane;
    }
}

namespace msw {
    namespace cockpit {

        // The spinning 3D coin drawn next to a team's coin counter. Lives in
        // the DOL. Size 0x18 (the element size CCockpitHockeyCoin passes to
        // __construct_array); the members are not mapped. Return types are
        // guesses.
        class C3DCoin {
            public:
                C3DCoin();
                ~C3DCoin();

                // Creates the coin of team nTeam under task pParent.
                void Initialize(long nTeam, bcs::CTask *pParent);
                void Finalize();
                // The coin's actions: back to rest, jump up (a coin gained),
                // fall (a coin lost).
                void ActReset();
                void ActPopup();
                void ActDrop();
                void EnableDisp(int bDisp);
                // Follows pPane on screen.
                void AttachPane(nw4r::lyt::Pane *pPane);

            private:
                unsigned char m_Unk00[0x18];
        };
    }
}

#endif /* !COCKPIT_C3DCOIN_HPP_ */
