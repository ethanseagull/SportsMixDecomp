#ifndef MSW_CCHRMANHOCKEY_HPP_
#define MSW_CCHRMANHOCKEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"
#include "game/msw/CShootMarker.hpp"

namespace msw {

    class CFighting;
    class CNpcBossPackun;
    struct FIGHTING_WORK;

    // The character manager of the hockey game (HockeyF), size 0xD0. It
    // makes the characters (eight), their drivers and markers, one shoot
    // marker per team and the fight over the puck (CFighting).
    // CHR_MAN_HOCKEY::CChrManHockeyCS derives from it for the party and
    // mission games.
    //
    // The virtual functions follow __vt__Q23msw13CChrManHockey; the last one,
    // GameGuideDispCheck_, is new in this class. CreateObj_, CreateDriver_
    // and CreateMarker_ are not virtual here (Create_ calls them directly and
    // the vtable has no slot for them); CChrManHockeyCS adds virtual ones of
    // the same names.
    class CChrManHockey : public CChrMan {
        public:
            // (guessed inline: CreateInstance builds the object in place)
            CChrManHockey(bcs::CTask *pParent) : CChrMan(pParent, 8)
            {
                m_nChrCoreNum = 8;
                for (long i = 0; i < 4; i++) {
                    m_ChgLeader[i].m_fWait = 0.0f;
                    m_ChgLeader[i].m_fKeep = 0.0f;
                    m_ChgLeader[i].m_nLast = -1;
                }
            }
            virtual ~CChrManHockey();

            // Makes the instance under pParent (the party game's class for
            // game types 2 and 3) and registers it.
            static CChrMan *CreateInstance(bcs::CTask *pParent);

            // Tells team nTeam's shoot marker to start over.
            virtual void ResetShootMarker(long nTeam) { m_pShootMarker[nTeam]->SetReset(); }
            virtual void MainFunc_();
            virtual void DispFunc();
            virtual void Create_();
            virtual void SetCamIdAssign_(long nNum, long *pId);
            virtual void AllReset_();
            virtual void InitLeader_(int bConnect);
            virtual CChrCore *Util_SearchChgLeader_(long nBall, long nTeam, CChrCore *pNow);
            virtual CChrCore *Util_SearchNearOwnGoal_(long nTeam, int, CChrCore *);
            virtual CChrCore *Util_SearchPassTo_(long nChr, float fRotY);
            virtual CChrCore *Util_SearchPassmeetTo_(long nChr, float fRotY, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_SearchArtsPassTo_(long nChr);
            virtual int Util_GetSubChrTargetPos_(long nChr, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_GetSubChrMarkChrCore_(CChrCore *);
            virtual void Util_SetChgLeaderOrder_(long);
            virtual long Util_GetNextChgLeader_(long);
            // Starts or ends the guide telop for the first human player that
            // asks for one.
            virtual void GameGuideDispCheck_();

            // Starts a fight over the puck between characters nChr1 and nChr2.
            void FightingStart_(long nChr1, long nChr2);
            // A copy of the fight's state.
            void GetFightingWork_(FIGHTING_WORK &rWork);
            // Adds fPower to character nChr's shaking; bFirst marks it as the
            // first to act (phase 2) unless another was. (names guessed)
            void FightingUpdateChrData_(long nChr, float fPower, int bFirst, int bJoin);
            // Hides the keepers' models while a special move plays.
            void ArtsStart_();

            // Makes the boss Packun nIdx at rPos (CStageHockey07::CreateGoal).
            // (return type guessed from the stage keeping the result, as
            // CChrManBasket's does)
            CNpcBossPackun *CreateBossPackun_(const bcs::FVEC3 &rPos, long nIdx);

            // The instance as this class (guessed inline: the callers call
            // CChrMan::GetInstance and use the result directly).
            static CChrManHockey *GetInstance() { return static_cast<CChrManHockey *>(CChrMan::GetInstance()); }
            // (name guessed) The fight of the instance.
            static CFighting *GetFighting() { return GetInstance()->m_pFighting; }

        protected:
            void CreateObj_();
            void CreateDriver_();
            void CreateMarker_();

            // What Util_SearchChgLeader_ keeps per team (names guessed).
            struct CHG_LEADER {
                float m_fWait; // frames before the leader may change again
                float m_fKeep; // frames m_nLast stays out of the choice
                long m_nLast;  // the player who was leader last, -1 for none
            };

            CHG_LEADER m_ChgLeader[4];          // 0x8C
            CShootMarker *m_pShootMarker[4];    // 0xBC, one per team
            CFighting *m_pFighting;             // 0xCC
    };
}

#endif /* !MSW_CCHRMANHOCKEY_HPP_ */
