#ifndef MSW_COCKPIT_CSPGAUGE_HPP_
#define MSW_COCKPIT_CSPGAUGE_HPP_

namespace bcs {
    namespace G2D {
        class CPaneCtrl;
    }
}

namespace msw {

    class CMenuPaneAccessor;

    namespace cockpit {

        // The special-shot gauge of the cockpit: plays a pane animation at a
        // frame that follows the gauge's fill rate. Only the members its
        // functions touch are known; the field names follow SetParam's
        // argument order and are guesses.
        class CSPGauge {
            public:
                void Initialize();
                void Finalize();
                void SetParam(bcs::G2D::CPaneCtrl *pPaneCtrl, unsigned short nPane, unsigned short nUnk02,
                              unsigned short nSubPane, unsigned char nAnim, unsigned char nMaxAnim,
                              unsigned char nSubAnim, unsigned char nSubMaxAnim);
                // Shows the gauge filled to fRate (clamped to [0, 1]).
                void SetGaugeAnim(CMenuPaneAccessor *pAccessor, float fRate);
                // Plays the "full" animations when bMax is set, else the
                // gauge at its last rate and the sub pane's normal one.
                void SetGaugeMaxAnim(CMenuPaneAccessor *pAccessor, int bMax);

            private:
                unsigned short m_nPane;      // 0x00, gauge pane, 0xFFFF when not set
                unsigned short m_nUnk02;     // 0x02, not read by these functions
                unsigned short m_nSubPane;   // 0x04, a second pane
                unsigned char m_nAnim;       // 0x06, gauge fill animation of m_nPane
                unsigned char m_nMaxAnim;    // 0x07, "full" animation of m_nPane
                unsigned char m_nSubAnim;    // 0x08, normal animation of m_nSubPane
                unsigned char m_nSubMaxAnim; // 0x09, "full" animation of m_nSubPane
                float m_fFrameMax;         // 0x0C, frames of animation m_nAnim
                float m_fRate;             // 0x10, last rate given to SetGaugeAnim
        };
    }
}

#endif /* !MSW_COCKPIT_CSPGAUGE_HPP_ */
