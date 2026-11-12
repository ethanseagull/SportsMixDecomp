#ifndef MSW_CCHRMANBASKET_HPP_
#define MSW_CCHRMANBASKET_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    class CChrCoreBasket;
    class CNpcBase;
    class CNpcBossPackun;
    class CNpcGhostBasketBase;

    // The character manager of the basketball game (BasketF), size 0xE8.
    // CChrManBasketHungryBossPackun (in game/basket/CChrManBasket.cpp) derives from it for the party
    // game against the Hungry Boss Packun.
    //
    // The virtual functions follow __vt__Q23msw13CChrManBasket; the last
    // slot (0xE8, the second Util_SearchPassTo_) is new in this class.
    // CreateObj_, CreateDriver_ and CreateMarker_ are not virtual here (Create_
    // calls them directly); the party game's class adds virtual ones of the
    // same names. The Util_ functions with a body here are emitted as weak
    // functions after the class's code in the original, in reverse order of
    // these declarations.
    class CChrManBasket : public CChrMan {
        public:
            // (guessed inline: CreateInstance builds the object in place)
            CChrManBasket(bcs::CTask *pParent) : CChrMan(pParent, 6)
            {
                m_pArtsChr = 0;
                m_nTeamChrNum = 0;
                m_nUnkA4 = 0;
                m_nChrCoreNum = 6;
                for (long i = 0; i < 4; i++) {
                    m_fPassDisableFrame[i] = 0.0f;
                    m_fChgInputDisableFrame[i] = 0.0f;
                    m_pPrevLeader[i] = 0;
                }
                m_nFlagAC = 0;
            }
            virtual ~CChrManBasket();

            // Makes the instance under pParent (the Hungry Boss Packun party
            // game's class for game type 2) and registers it.
            static CChrMan *CreateInstance(bcs::CTask *pParent);

            virtual void ChrCoreMainCall_();
            virtual void CheckPassMode_(CChrCore *pChr);
            virtual void MainFunc_();
            virtual void Create_();
            virtual void AllReset_();
            virtual void InitLeader_(int bConnect);
            virtual void AllSendMsg_(CChrDrv::MSG_ID id, long, long, unsigned long, unsigned long);
            virtual void AllSetState_(long nState, long, long);
            virtual int CanbeTurnOver_();
            virtual void StartScoreView_(long nTeam);
            virtual void AllStop_(CChrCore::STOP_MODE, CChrCore::STOP_MODE);
            virtual void SendMessage_(long nMsg, long, long, long);
            virtual CChrCore *Util_SearchChgLeader_(long, long, CChrCore *);
            virtual CChrCore *Util_SearchNearOwnGoal_(long nTeam, int, CChrCore *);
            virtual CChrCore *Util_SearchPassTo_(long nChr, float fRotY);
            virtual CChrCore *Util_SearchPassmeetTo_(long nChr, float fRotY, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_SearchArtsPassTo_(long) { return 0; }
            virtual int Util_GetSubChrTargetPos_(long, bcs::FVEC3 &) { return 0; }
            virtual CChrCore *Util_GetSubChrMarkChrCore_(CChrCore *) { return 0; }
            virtual long Util_GetNextChgLeader_(long) { return 0; }
            virtual void Util_SetChgLeaderOrder_(long) {}
            // The team mate of character nChr nearest to the direction fRotY
            // (*ppChr), or on the ghost stage the ghost when it is nearer
            // (*ppNpc); rPos gets where to aim the pass.
            virtual void Util_SearchPassTo_(CChrCore **ppChr, CNpcBase **ppNpc, long nChr, float fRotY,
                                            bcs::FVEC3 &rPos);                         // 0xE8

            // The boss Packun whose index (the NPC's virtual at 0x80) is
            // nIdx; null when there is none.
            CNpcBossPackun *GetBossPackun_(long nIdx) const;
            // Makes the boss Packun nIdx at rPos (the Hungry Boss Packun
            // stages call it for the boss that carries goal 0).
            CNpcBossPackun *CreateBossPackun_(const bcs::FVEC3 &rPos, long nIdx);

            // The instance as this class (guessed inline: the callers call
            // CChrMan::GetInstance and use the result directly).
            static CChrManBasket *GetInstance() { return static_cast<CChrManBasket *>(CChrMan::GetInstance()); }

            // Added for game/basket/CChrDrvPadBasket.cpp (names guessed).
            // Blocks a change of character by team nTeam's pad for a while
            // (sets m_fChgInputDisableFrame[nTeam]).
            void OnChgCharacterInputDiableFrame_(long nTeam);
            // Starts a combination play of pChr's team; zero when none
            // starts.
            int PlayCombination_(const CChrCore *pChr);
            // Not zero when team nTeam may change its character by pad.
            // (guessed inline: the drivers read the member after calling
            // CChrMan::GetInstance)
            static int IsChgCharacterInputEnable(long nTeam)
            {
                return GetInstance()->m_fChgInputDisableFrame[nTeam] <= 0.0f;
            }
            // (name guessed, after the one above) Not zero when a pass may
            // change team nTeam's character again.
            static int IsChgCharacterPassEnable(long nTeam)
            {
                return GetInstance()->m_fPassDisableFrame[nTeam] <= 0.0f;
            }

            // Added for game/basket/CNpcGhostBasketBase.cpp (names from the
            // DOL symbols; return types and meanings guessed). Makes the
            // ghost of type nType for team nTeam.
            CNpcGhostBasketBase *CreateGhost_(long nType, long nTeam);
            // Team nTeam's ghost; null when there is none.
            CNpcBase *GetGhost_(long nTeam);
            // Starts the extra time (when the ghost takes the ball from the
            // team that is not the player's).
            void SetClockOverTime_();
            // (name guessed) Zero while bit 1 of the word at 0xAC is set:
            // the ghost does not come back then.
            bool IsGhostAppearEnable() const { return !(m_nFlagAC & 2); }
            // (added for game/basket/CGameBasketHungryBossPackun.cpp; names
            // guessed) Ends the extra time; sets flag bits of m_nFlagAC
            // (bit 0 is set by the party game's wait-start state for stage
            // 1, meaning not known).
            void ResetClockOver_();
            void SetFlagAC(unsigned long nFlag) { m_nFlagAC |= nFlag; }
            // (added for game/basket/unit_31D0C.cpp) The flag bits of m_nFlagAC.
            unsigned long GetFlagAC() const { return m_nFlagAC; }
            // (name guessed; inline in CComManBasketGame::GetBaseShootPoint)
            // Bit 0x10 of m_nFlagAC: the game has no shot clock.
            unsigned long IsNoShotClock() const { return m_nFlagAC & 0x10; }

            // Added for game/basket/CComManBasket.cpp (return types
            // guessed). Not zero when the boss Packun has been made.
            int IsCreateBossPackun_() const;
            // Not zero when every character of team nTeam is computer-driven.
            int IsAllComTeam_(long nTeam) const;
            // The yaw from rPos1 towards rPos2 in the XZ plane.
            static float GetRotY_Pos1ToPos2XZ(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2);

            // Added for game/basket/CChrManBasket.cpp (names from the
            // symbols; return types and parameter meanings guessed).
            // Not zero when pChr cannot take a pass: none, out of the game,
            // status bit 0x10000, state 6, or a level word of 1 or 4.
            static int IsBadConditon(const CChrCore *pChr);
            // Asks whoever holds the ball (a character of pChr's team, or an
            // NPC of type 3 to 5) to pass it to pChr.
            void Util_SendPassRequest_(long nTeam, const CChrCore *pChr);
            // Not zero when rPos3 is in the face (rPos1, rPos2, fA, fB) and
            // within fRange of the direction from rPos1 to rPos2.
            int InRectAndRad_(const bcs::FVEC3 &rPos1, const bcs::FVEC3 &rPos2, const bcs::FVEC3 &rPos3, float fA,
                              float fB, float fRange) const;
            // Starts the slow camera of a trick move when pChr's move asks
            // for one.
            void CheckCameraSlowTrickMove_(float fRate, const CChrCore *pTarget, CChrCoreBasket *pChr);
            // Not zero when rPos is in the three-point area of goal nGoal,
            // the line widened by fMargin.
            int In3PoinArea_(long nGoal, const bcs::FVEC3 &rPos, float fMargin) const;
            // Blocks a change of character by pass for pChr's team for a
            // while (longer while bit 0x800 of a player's flags is set).
            void OnChgCharacterPassDisableFrame_(const CChrCore *pChr);
            // The cockpit's head icon over pChr (over the team's previous
            // leader when there is one), for the human players.
            void DispPlayerIcon_(const CChrCore *pChr);
            void DispPlayerIconSetCore_(const CChrCore *pChr);
            void AllDispPlayerIcon_();
            void ClearPlayerIcon_(const CChrCore *pChr, int);
            void AllClearPlayerIcon_(int);
            // (added for game/basket/CChrCoreBasketNinja.cpp; names guessed)
            // The two above on the instance: weak copies of
            // ClearPlayerIcon__Q23msw13CChrManBasketFPCQ23msw8CChrCorei are in
            // BasketF.
            static void ClearPlayerIcon(const CChrCore *pChr, int n) { GetInstance()->ClearPlayerIcon_(pChr, n); }
            static void DispPlayerIconSetCore(const CChrCore *pChr) { GetInstance()->DispPlayerIconSetCore_(pChr); }
            // Not zero when a pad drives pChr (driver type 1).
            int IsPadTypePlayer_(const CChrCore *pChr) const;
            // The shot clock (the game work's word at 0x30): in seconds,
            // rounded up, and in frames.
            long GetClockTimeSeconds_() const;
            float GetClockTime_() const;
            // Counts the shot clock down while it runs (bit 0 of
            // m_nFlagAC) and nobody shoots.
            void UpDateShotClockOver_(float);
            // pChr's special shot (air trick) begins: the arts event starts.
            void StartAirTrick_(CChrCore *pChr);
            // (meaning guessed) A value for arts curve type nType.
            float GetArtsCurveValue_(long nType) const;
            // The frames left before boss Packun nIdx closes its mouth.
            float GetRemainderMouthOpenFrame_(long nIdx);
            // The motion frames of a just shot of pChr, earliest and latest,
            // by its distance from the goal.
            long GetJustShotTimingMin_(const CChrCoreBasket *pChr) const;
            long GetJustShotTimingMax_(const CChrCoreBasket *pChr) const;
            // Not zero when pChr's shots count in the score book.
            int IsScoreBookCountChr_(const CChrCore *pChr) const;
            // The position of goal nGoal's ring; zero when the goal has no
            // model.
            int GetRingCSetPos_(long nGoal, bcs::FVEC3 &rPos) const;
            // The quarter (0 to 3) a yaw falls in, for the dunk battle.
            long GetDir4ToBattle_(float fRotY) const;
            // Lets go of goal nGoal's rim after fFrame frames.
            void SetDunkRelease_(long nGoal, float fFrame);
            // pChr was team nTeam's leader before the change.
            void SetPrevLeader_(long nTeam, const CChrCore *pChr);

        protected:
            void CreateObj_();
            void CreateDriver_();
            void CreateMarker_();

            // 0x8C (name guessed): per team, the character that led it before
            // the last change (SetPrevLeader_); the head icon stays over it.
            const CChrCore *m_pPrevLeader[4];
            // 0x9C (name guessed): the character whose special shot plays
            // (StartAirTrick_); MainFunc_ ends the arts event with it.
            CChrCoreBasket *m_pArtsChr;
            // 0xA0 (name guessed): the characters per team (1 to 3, by the
            // game work's byte at 0x06).
            long m_nTeamChrNum;
            long m_nUnkA4;              // 0xA4, cleared by the constructor only
            long m_nDunkGoal;           // 0xA8, see SetDunkRelease_
            // 0xAC (name guessed): flag bits. 0x1: the shot clock runs; 0x2
            // and 0x4: an arts event plays (StartAirTrick_); 0x8: the clock is
            // reset on message 0x13; 0x10: no shot clock; 0x40: a dunk
            // release is pending (SetDunkRelease_).
            unsigned long m_nFlagAC;
            unsigned char m_UnkB0[0xB4 - 0xB0];
            // 0xB4 (name guessed): per team, frames since the leader changed
            // (SetPrevLeader_ sets 40 or 0).
            float m_fPrevLeaderFrame[4];
            // 0xC4 (name guessed): per team, the frames left before a pass
            // changes the character again (OnChgCharacterPassDisableFrame_).
            float m_fPassDisableFrame[4];
            // 0xD4 (name guessed): per team, the frames left before the pad
            // may change character again (OnChgCharacterInputDiableFrame_
            // writes it).
            float m_fChgInputDisableFrame[4];
            float m_fDunkReleaseFrame;  // 0xE4, see SetDunkRelease_
    };
}

#endif /* !MSW_CCHRMANBASKET_HPP_ */
