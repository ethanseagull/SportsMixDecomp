#ifndef MSW_CBODYCOLLISION_HPP_
#define MSW_CBODYCOLLISION_HPP_

namespace msw {

    // A set of body collision units, 0xC bytes (msw::CItem embeds one at
    // 0x17C). A DOL class.
    //
    // INCOMPLETE: only what game/boss/CItemManBoss.cpp needs; the three
    // words are not mapped.
    class CBodyCollision {
        public:
            // Target flags of every unit: set all to `flag`, or add `flag`.
            void ResetTargetFlagAll(unsigned long flag);
            void AddTargetFlagAll(unsigned long flag);

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CBODYCOLLISION_HPP_ */
