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

            // (added for BasketF CChrCoreBasketKuroma; parameter meanings
            // guessed) Makes nNum units; sets on every unit the attack type,
            // the target flags, the flags of what it does not hit, the
            // radius, the position, the effect played on a hit and the
            // owner's index; turns every unit on.
            void Create(long nNum, long, long, unsigned long);
            void SetAtkTypeAll(unsigned long nType);
            void SetTargetFlagAll(unsigned long nFlag);
            void SetNoHitFlagAll(unsigned long nFlag);
            void SetRadiusAll(float fRadius);
            void SetPosAll(const bcs::FVEC3 &rPos);
            void SetHitEffectIDAll(long nEffect);
            void SetIndexAll(long nIndex);
            void SetEnableAll();

            // Per unit (added for game/msw/CStgGmk_Trolley.cpp; DOL
            // functions, argument meanings guessed). SetPos2 is the other
            // end of a capsule.
            void SetCollType(long index, long type);
            void SetTargetFlag(long index, unsigned long nFlag);
            void SetAtkType(long index, unsigned long nType);
            void SetEnable(long index);
            void SetRadius(long index, float fRadius);
            void SetPos(long index, const bcs::FVEC3 &rPos);
            void SetPos2(long index, const bcs::FVEC3 &rPos);

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CATKCOLLISION_HPP_ */
