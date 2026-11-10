#ifndef MSW_CBODYCOLLISION_HPP_
#define MSW_CBODYCOLLISION_HPP_

namespace bcs {
    struct FVEC3;
}

namespace msw {
    class CBall;
    class CCollAtkUnit;
    class CCollBodyUnit;

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

            // Callbacks of every unit; pArg is what was registered. The
            // damage callback's result goes back to the attacker (meaning
            // not known).
            typedef void (*BodyPushCallback)(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg,
                                             bcs::FVEC3 *pPush);
            typedef unsigned long (*BodyDamageCallback)(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                        const bcs::FVEC3 &rPos, void *pArg);
            typedef void (*BallCatchCallback)(CCollBodyUnit *pBody, CBall *pBall, void *pArg);
            void SetBodyPushCallbackAll(BodyPushCallback pFunc, void *pArg);
            void SetBodyDamageCallbackAll(BodyDamageCallback pFunc, void *pArg);
            void SetBallCatchCallbackAll(BallCatchCallback pFunc, void *pArg);

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CBODYCOLLISION_HPP_ */
