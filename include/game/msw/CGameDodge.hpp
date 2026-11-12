#ifndef CGAMEDODGE_HPP_
#define CGAMEDODGE_HPP_

#include "game/msw/CGame.hpp"

namespace msw {

    // Both live in module DodgeF.

    // Size 0x420.
    class CGameDodge : public CGame {
        public:
            CGameDodge(bcs::CTask *pParent);

            // Lives left to character nChr (static: the body works on
            // CGame::m_pInstance). CChrMarkerDodge shows no marker in the
            // party game for a character with none left.
            static long GetChrRestLife(long nChr);
            // (static, DodgeF; meaning guessed from the names) The share of
            // its life character nChr has left, and whether an arts shot is
            // being played.
            static float GetChrRestLifeRate(long nChr);
            static int IsNowArtsShootMode();
            // (static, DodgeF; added for game/dodge/CCockpitDodge.cpp,
            // meaning guessed) The number of characters of team nTeam; the
            // result screen shows that many players per team.
            static long GetVariableChrNum(long nTeam);
            // (static, DodgeF; added for game/dodge/CCockpitDodgeCoder.cpp,
            // which ignores the result; return type guessed) The life a
            // character of team nTeam starts with.
            static long GetChrMaxLife(long nTeam);
            // (static, DodgeF; added for game/dodge/CChrDrvPadDodge.cpp)
            // Hands the pad of character nFrom to character nTo.
            static void ChrChange(long nTo, long nFrom);

            // The dodgeball game's own virtual functions from slot 0x2A8 (after
            // CGame's), in the order of __vt__Q23msw10CGameDodge; only as far
            // as CTmGameHandlerDodge needs. Return types are placeholders
            // (void) except where a body or caller shows them.
            virtual void GetPassCount_() const;                          // 0x2A8
            virtual void IncPassCount_(long);                            // 0x2AC
            virtual void ClearPassCount_();                              // 0x2B0
            virtual void SetPassCatch_(long);                            // 0x2B4
            virtual void TeamNoConv_(long) const;                        // 0x2B8
            virtual void ChrChange_(long, long);                         // 0x2BC
            virtual void UpdatePowerAll_();                              // 0x2C0
            virtual void PowerUpTelopIn_(long);                          // 0x2C4
            virtual void UpdatePowerUpTelop_(float);                     // 0x2C8
            virtual void InitScoreAll_();                                // 0x2CC
            virtual long GetChrLostScore_(long);                         // 0x2D0
            // GetChrLostScore_: the word at 0x1B4 + 4 * nChr (the score
            // character nChr has lost; meaning guessed from the name).
            virtual long GetChrGetScore_(long);                          // 0x2D4
            // (added for game/dodge/CChrCoreDodgeDodgeOrDie.cpp) Adds to
            // the score character nChr has lost; the party game passes
            // the damage taken and -1.
            virtual void AddChrLostScore_(long nChr, long nScore, long);  // 0x2D8
            // (added for game/dodge/unit_3AAC8.cpp, which calls TrgOut_ when
            // a character falls out; names and arguments from the vtable's
            // symbols, return types guessed)
            virtual void AddChrGetScore_(long, long);                    // 0x2DC
            virtual long GetChrLostLife_(long) const;                    // 0x2E0
            virtual long GetChrGetLife_(long) const;                     // 0x2E4
            virtual long GetChrMaxLife_(long) const;                     // 0x2E8
            virtual long GetChrMaxLife__(long) const;                    // 0x2EC
            virtual long GetChrRebirthLife_(long) const;                 // 0x2F0
            virtual void AddChrLostLife_(long, long, long);              // 0x2F4
            virtual void AddChrGetLife_(long, long, long);               // 0x2F8
            virtual int IsChrInner_(long) const;                         // 0x2FC
            virtual void UpdateArrangeCode_();                           // 0x300
            virtual float GetTeamRestLifeRate(long) const;               // 0x304
            virtual float GetTeamPeriodRestLifeRate(long, long) const;   // 0x308
            virtual void SetTeamPeriodRestLifeRate(long, long, float);   // 0x30C
            virtual float GetMaxShotTimeRaw_() const;                    // 0x310
            virtual int IsNowArtsShootMode_() const;                     // 0x314
            virtual int IsDodgeArtsCatchModeByTeam_(long) const;         // 0x318
            virtual long GetMinOuterNum_() const;                        // 0x31C
            virtual long GetInitialOuterNum_() const;                    // 0x320
            virtual void PushAppSpeed_(float);                           // 0x324
            virtual void PopAppSpeed_();                                 // 0x328
            virtual int IsChrExclusive_(long) const;                     // 0x32C
            virtual void SetLifeCoderAll_() const;                       // 0x330
            virtual void SetLifeCoder_(long, long, long) const;          // 0x334
            virtual void AddLifeCoder_(long, long, long, long, long, long); // 0x338
            virtual void UpdateJudgeLifeCoderAll_();                     // 0x33C
            virtual float DoDGetChrLockonFrame_(long) const;             // 0x340
            virtual void DoDSetChrLockonFrame_(long, float);             // 0x344
            virtual void LineMsgInit_();                                 // 0x348
            virtual void LineMsgReset_();                                // 0x34C
            virtual void LineMsgIncAttackNum_(long);                     // 0x350
            virtual void LineMsgUpdate_();                               // 0x354
            virtual void InitBasicGuide();                               // 0x358
            virtual void ResetBasicGuide();                              // 0x35C
            virtual int IsDispBasicGuide_() const;                       // 0x360
            virtual int IsEnableBasicGuide() const;                      // 0x364
            virtual void UpdateBasicGuide(float);                        // 0x368
            virtual void UpdateBasicGuide_JumpBall();                    // 0x36C
            virtual void UpdateBasicGuide_Uncatchable();                 // 0x370
            // A character of team nTeam is out (the falling into the river
            // passes -1, the other team, 0, 1, its place and its number).
            virtual void TrgOut_(long, long nTeam, long, long, float, const bcs::FVEC3 &rPos, long nChr); // 0x374

        private:
            unsigned char m_Unk150[0x420 - sizeof(CGame)];
    };

    // The dodgeball party game. Size 0x4A8.
    class CGameDodgeDodgeOrDie : public CGameDodge {
        public:
            CGameDodgeDodgeOrDie(bcs::CTask *pParent);

        private:
            unsigned char m_Unk420[0x4A8 - 0x420];
    };
}

#endif /* !CGAMEDODGE_HPP_ */
