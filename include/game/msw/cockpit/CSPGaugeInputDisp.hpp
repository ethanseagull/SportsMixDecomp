#ifndef MSW_COCKPIT_CSPGAUGEINPUTDISP_HPP_
#define MSW_COCKPIT_CSPGAUGEINPUTDISP_HPP_

namespace msw {

    class CMenuPaneAccessor;

    namespace cockpit {

        // The input prompt shown on the special-shot gauge: which buttons to
        // press, for a remote held alone or with a nunchaku. Lives in the
        // DOL (game/msw/cockpit/CMissionCount.cpp). Size 0xC, vptr at 0x08
        // (its vtable and inline destructor are emitted by the modules).
        // Field names follow SetParam's argument order and SetDispAnim.
        class CSPGaugeInputDisp {
            public:
                // Inline, emitted weak by the modules, as CSPGauge's.
                CSPGaugeInputDisp() { Initialize(); }

                void Initialize();
                void Finalize();
                void SetParam(unsigned char nTeam, unsigned short nPane, unsigned char nAnim,
                              unsigned short nNunchakuPane, unsigned short nRemotePane);
                // Shows the prompt for the pad of team nTeam's player (when
                // bDisp is set and a pad drives him), else hides it.
                void SetDispAnim(CMenuPaneAccessor *pAccessor, int bDisp);

            private:
                unsigned short m_nPane;         // 0x00, the prompt
                unsigned short m_nNunchakuPane; // 0x02, shown with a nunchaku
                unsigned short m_nRemotePane;   // 0x04, shown for a remote alone
                unsigned char m_nTeam;          // 0x06, passed to CGame::GetTeamColorIdx
                unsigned char m_nAnim;          // 0x07, played on m_nPane; 0xFF after Initialize

            public:
                // vptr at 0x08: declared after the data.
                virtual ~CSPGaugeInputDisp() { Finalize(); }
        };
    }
}

#endif /* !MSW_COCKPIT_CSPGAUGEINPUTDISP_HPP_ */
