#ifndef MSW_CCOMDRVHOCKEY_HPP_
#define MSW_CCOMDRVHOCKEY_HPP_

#include "game/msw/CComDrv.hpp"
#include "game/msw/CComManHockey.hpp"

namespace msw {

    // The computer driver of the hockey game (HockeyF), 0x1B0 bytes (its
    // members end at 0x1AC; no allocation site was found).
    //
    // Virtual functions in the order of __vt__Q23msw13CComDrvHockey: the
    // overrides of CComDrv's slots, then three of its own (0xE0-0xE8).
    // OnInit, OnMain, MethdSkate and MethdRotateY(float, long) are not in
    // game/hockey/CComDrvHockey.cpp: they follow its weak functions in the
    // original and have their own float constants (a second 0.0f), so they
    // belong to another file. Return types other than the vtable's are
    // guesses.
    class CComDrvHockey : public CComDrv {
        public:
            // What the driver keeps of one keeper (names guessed).
            struct KEEPER_INFO {
                CChrCore *m_pChr;        // null when there is none
                long m_nPlayerNo;        // its CChrCore::GetPlayerNo(), -1 for none
                bcs::FVEC3 m_Pos;        // CComMan::GetChrPos
                bcs::FVEC3 m_WorldPos;   // CComMan::GetChrWorldPos
            };

            CComDrvHockey(long nArg);
            virtual ~CComDrvHockey();

            virtual int MethdRotateY(float fRot, long);
            virtual void ClearAll();
            virtual void UpdateInfo();
            virtual long OnMain(_CHR_MSG *pMsg);
            virtual long OnInit(_CHR_MSG *pMsg);
            virtual int GetMapStatus(short nX, short nZ, CComManHockey::MAP_STATUS_DATA &rData);
            virtual int GetPartyMapStatus(short nX, short nZ, unsigned char &, unsigned char &, char &);
            virtual int MethdSkate(const bcs::FVEC3 &pos, int, float);

            // The computer driver of pChr; null when a computer does not
            // drive it.
            CComDrvHockey *GetComDrv(const CChrCore *pChr) const;
            long GetChrTypeID() const;
            // CComManHockey's game time; zero outside the hockey game.
            float GetElapsedTime() const;
            void InitMyTeamKeeperCore();
            CChrCore *GetMyTeamKeeperCore() const;
            void InitEmTeamKeeperCore();
            CChrCore *GetEmTeamKeeperCore() const;
            int CanReachTarget(long, int, const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float);
            int isExistPillar();

        protected:
            KEEPER_INFO m_EmKeeper; // 0x170, the other team's keeper
            KEEPER_INFO m_MyKeeper; // 0x190, the own team's keeper
    };
}

#endif /* !MSW_CCOMDRVHOCKEY_HPP_ */
