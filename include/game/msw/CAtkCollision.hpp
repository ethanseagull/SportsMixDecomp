#ifndef MSW_CATKCOLLISION_HPP_
#define MSW_CATKCOLLISION_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {
    class CCollAtkUnit;
    class CCollBodyUnit;

    // A set of attack collision units, 0xC bytes (msw::CItem embeds one at
    // 0x170). A DOL class.
    //
    // INCOMPLETE: only what game/boss/CItemManBoss.cpp needs; the three
    // words are not mapped.
    class CAtkCollision {
        public:
            // Added for the DOL's msw::CStgGmk_WaterPillar_Act.
            CAtkCollision();
            ~CAtkCollision();
            void SetDisableAll();

            // The radius of unit `index`.
            float GetRadius(long index) const;
            // (name guessed; the DOL's fn_80109170) The float at 0xA0 of
            // unit `index` (units are 0xAC bytes); 0 for an index out of
            // range.
            float GetUnkA0(long index);

            // Sets the attack power of every unit.
            void SetAtkPowerAll(float power);

            // Called when a unit hits a body; pArg is what was registered.
            typedef void (*AttackHitCallback)(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                              const bcs::FVEC3 &rPos, unsigned long flag, void *pArg);
            void SetAttackHitCallbackAll(AttackHitCallback pFunc, void *pArg);

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CATKCOLLISION_HPP_ */
