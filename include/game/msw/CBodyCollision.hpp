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
            // Added for BossF game/boss/CChrCoreBossKinopio.cpp (DOL;
            // argument meanings guessed): nUnit units of collision kind
            // nKind.
            CBodyCollision(long, long nUnit, long nKind, unsigned long);
            ~CBodyCollision();
            void SetTargetFlagAll(unsigned long flag);
            void SetPushRankAll(long rank);
            void SetPushPowerAll(long power);
            void SetPosAll(const bcs::FVEC3 &pos);
            void AddPosXZAll(const bcs::FVEC3 &pos, const bcs::FVEC3 &move);
            void SetSizeAll(float fRadius, float fHeight, float);
            void SetPushEnableAll();
            void SetEnableAll();
            // Recomputes the bounding boxes after a move.
            void CalcAABBAll();

            // Target flags of every unit: set all to `flag`, or add `flag`.
            // Added for the DOL's msw::CStgGmk_WaterPillar_Act (parameter
            // meanings guessed).
            CBodyCollision();
            void Create(long num, long, long, unsigned long);
            void SetCollType(long index, long type);
            void SetDisableAll();
            void SetPos(long index, const bcs::FVEC3 &rPos);
            void SetSize(long index, const bcs::FVEC3 &rSize);
            void SetSize(long index, float x, float y, float z);
            void SetPushEnable(long index);

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

            // (DOL; used by DodgeF's bombs, argument meanings guessed)
            void SetPushRank(long index, long rank);
            void SetPushPower(long index, long power);
            void SetBodyEnable(long index);
            void SetTargetFlag(long index, unsigned long flag);
            void SetRadius(long index, float radius);
            void SetBodyDisableAll();
            // (added for VolleyF's CC09_VOLLEY::CcKinopioBoundMushroom)
            void SetHitEffectFrameAll(float fFrame);

            // Added for BasketF game/basket/CNpcGhostBasketBase.cpp (all in
            // the DOL; argument meanings guessed). The ghost makes four
            // units with the constructor above (BasketF read its arguments
            // as nUnit, nType, ?, ?; BossF's reading kept). The
            // constructor, SetCollType, SetPos, SetSize, SetPushEnable,
            // SetPosAll, AddPosXZAll and CalcAABBAll it also uses are
            // declared above.
            void SetEnable(long index);
            void SetDisable(long index);
            void SetCatchEnable(long index);
            void ResetTargetFlag(long index, unsigned long flag);
            void SetBallCatchCallback(long index, BallCatchCallback pFunc, void *pArg);
            void SetYRot(long index, float fRotY);
            float GetRadius(long index) const;

        private:
            unsigned long m_Unk00[3];
    };
}

#endif /* !MSW_CBODYCOLLISION_HPP_ */
