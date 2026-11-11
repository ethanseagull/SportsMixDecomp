#ifndef MSW_CCHRMANHOCKEYCS_HPP_
#define MSW_CCHRMANHOCKEYCS_HPP_

#include "game/msw/CChrManHockey.hpp"
#include "game/msw/CTournament.hpp"

namespace msw {
    namespace CHR_MAN_HOCKEY {

        // The character manager of the hockey party game and of the
        // tournament's extra (mission) match, size 0xD4 (CreateInstance
        // makes it for game types 2 and 3). Four characters, one per team,
        // or six in the mission match. Its functions follow CChrManHockey's
        // in game/hockey/CChrManHockey.cpp, one file with it: its vtable is
        // in that file's data, ahead of CFighting's and CChrManHockey's.
        class CChrManHockeyCS : public CChrManHockey {
            public:
                // (guessed inline: CreateInstance builds the object in place)
                CChrManHockeyCS(bcs::CTask *pParent) : CChrManHockey(pParent)
                {
                    if (CTournament::IsExtraMatchGame()) {
                        m_nChrCoreNum = 6;
                        m_bMission = 1;
                    } else {
                        m_nChrCoreNum = 4;
                    }
                }
                virtual ~CChrManHockeyCS();

                virtual void DispFunc();
                virtual void Create_();
                virtual void AllReset_();
                virtual void InitLeader_(int bConnect);
                virtual void AllSetState_(long nState, long, long);
                virtual int CanbeTurnOver_();
                // How many characters the game has.
                virtual long CheckGenChrCoreNum_();
                virtual void CreateDriver_();
                virtual void CreateMarker_();

            private:
                // The drivers of the party game (KS) and of the mission
                // match.
                void KSCreateDriver_();
                void MissionCreateDriver_();

                long m_bMission; // 0xD0, the tournament's extra match (name guessed)
        };
    }
}

#endif /* !MSW_CCHRMANHOCKEYCS_HPP_ */
