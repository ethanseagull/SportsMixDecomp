#ifndef MSW_CCOLLATKUNIT_HPP_
#define MSW_CCOLLATKUNIT_HPP_

namespace msw {

    // One attack collision unit (0xAC bytes; derived from msw::CCollUnit,
    // whose SetPos and SetAtkType msw::CBombExplosion::Create_ calls on it).
    // A DOL class.
    //
    // INCOMPLETE: only the flag word msw::CBombExplosionBoss::Create_ sets.
    class CCollAtkUnit {
        public:
            unsigned char m_Unk00[0x18];
            unsigned long m_nFlag; // 0x18, CBombExplosion::Create_ sets 0x2A
    };
}

#endif /* !MSW_CCOLLATKUNIT_HPP_ */
