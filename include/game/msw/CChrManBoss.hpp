#ifndef MSW_CCHRMANBOSS_HPP_
#define MSW_CCHRMANBOSS_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    // The character manager of the boss game (BossF), size 0xAC. It makes
    // the players' characters (up to eight), their drivers and markers, and
    // the boss itself (CNpcBehemothBoss).
    //
    // The virtual functions follow __vt__Q23msw11CChrManBoss. The five with
    // a body here are emitted as weak functions after the file's code, so
    // they are inline; the last three (CheckGenChrCoreNum_, CreateDriver_,
    // CreateMarker_) are new in this class: the DOL's CChrMan vtable ends
    // before them.
    class CChrManBoss : public CChrMan {
        public:
            // (guessed inline: CreateInstance builds the object in place)
            CChrManBoss(bcs::CTask *pParent) : CChrMan(pParent, 8)
            {
                for (long i = 0; i < 8; i++) {
                    m_nDrvId[i] = 0;
                }
            }
            virtual ~CChrManBoss();

            // Makes the instance under pParent and registers it.
            static CChrMan *CreateInstance(bcs::CTask *pParent);

            virtual void DispFunc();
            virtual void Create_();
            virtual void AllReset_();
            virtual void InitLeader_(int bConnect);
            virtual int CanbeTurnOver_();
            virtual CChrCore *Util_SearchChgLeader_(long, long, CChrCore *) { return 0; }
            virtual CChrCore *Util_SearchNearOwnGoal_(long, int, CChrCore *) { return 0; }
            virtual CChrCore *Util_SearchPassTo_(long nChr, float fRate);
            virtual CChrCore *Util_SearchPassmeetTo_(long nChr, float fRate, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_SearchArtsPassTo_(long nChr);
            virtual int Util_GetSubChrTargetPos_(long nChr, bcs::FVEC3 &rPos);
            virtual CChrCore *Util_GetSubChrMarkChrCore_(CChrCore *) { return 0; }
            virtual void Util_SetChgLeaderOrder_(long) {}
            virtual long Util_GetNextChgLeader_(long) { return 0; }
            // How many characters the game has.
            virtual long CheckGenChrCoreNum_();
            virtual void CreateDriver_();
            virtual void CreateMarker_();

        private:
            // Makes the character objects (not virtual: Create_ calls it
            // directly).
            void CreateObj_();

            // 0x8C (name guessed): the driver kind of each character, from
            // SetDrvIdAssign_. 1 starts on the pad driver; 3 and the teams
            // of control type 1 use the computer driver.
            long m_nDrvId[8];
    };
}

#endif /* !MSW_CCHRMANBOSS_HPP_ */
