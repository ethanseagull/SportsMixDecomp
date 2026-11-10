#ifndef MSW_CATKCOLLISION_HPP_
#define MSW_CATKCOLLISION_HPP_

namespace msw {

    // A set of attack collision units, 0xC bytes (msw::CItem embeds one at
    // 0x170). A DOL class.
    //
    // INCOMPLETE: only what game/boss/CItemManBoss.cpp needs; the three
    // words are not mapped.
    class CAtkCollision {
        public:
            // Sets the attack power of every unit.
            void SetAtkPowerAll(float power);

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CATKCOLLISION_HPP_ */
