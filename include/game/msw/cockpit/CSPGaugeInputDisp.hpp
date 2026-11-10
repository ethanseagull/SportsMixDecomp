#ifndef MSW_COCKPIT_CSPGAUGEINPUTDISP_HPP_
#define MSW_COCKPIT_CSPGAUGEINPUTDISP_HPP_

namespace msw {
    namespace cockpit {

        // The input prompt shown on the special-shot gauge. Only the eight
        // bytes its three functions touch are known; field names follow
        // SetParam's argument order.
        class CSPGaugeInputDisp {
            public:
                void Initialize();
                void Finalize();
                void SetParam(unsigned char kind, unsigned short frame0, unsigned char alpha,
                              unsigned short frame1, unsigned short frame2);

            private:
                unsigned short m_Frame0; // 0x00
                unsigned short m_Frame1; // 0x02
                unsigned short m_Frame2; // 0x04
                unsigned char m_Kind;    // 0x06
                unsigned char m_Alpha;   // 0x07
        };
    }
}

#endif /* !MSW_COCKPIT_CSPGAUGEINPUTDISP_HPP_ */
