#ifndef MSW_CTMGAMEHANDLER_HPP_
#define MSW_CTMGAMEHANDLER_HPP_

#include "bcs/CUnivTask.hpp"
#include "game/msw/CGameSnd.hpp"

namespace msw {

    // What a tournament match adds to a game: the mission or battle rules,
    // their counters and the check for the end of the match. A DOL class,
    // size 0x44: bcs::CUnivTask at 0x00, own members from 0x34. Each game
    // module derives its own handler from it.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw18CTmGameHandlerBoss (BossF). Bodies given here are the
    // ones the modules emit as weak functions (the DOL's copies have no
    // name); the functions without a body are DOL functions. Return types
    // are not part of the mangled names: "int" stands for a function whose
    // body returns zero and whose callers in the DOL test or store the
    // value, "void" for an empty body; all are guesses.
    class CTmGameHandler : public bcs::CUnivTask {
        public:
            CTmGameHandler(bcs::CTask *pParent);
            virtual ~CTmGameHandler();
            virtual void Main();
            virtual void Disp();

            // A message of the game (the arguments of msw::CGame::SendMessage).
            virtual void _SendMessage(long, long, long, long) {}
            virtual void _CheckPeriod() {}
            virtual int _CheckWin() { return 0; }
            virtual int _CheckClear() { return 0; }
            virtual int _CheckAppeal(int) { return 0; }
            virtual int _CheckBallHold(long, long) { return 0; }
            virtual int _CheckShotClockTimeOver() { return 0; }
            virtual int _IsClear(long) { return 0; }
            virtual int _IsFaild(long) { return 0; }
            virtual int _IsMissionShootFalseRetry(long, long) { return 0; }
            virtual void _SetCountDisp(int) {}
            virtual void _SetCountUp(long) {}
            virtual void _SeCountUp() { CGameSnd::Play(0xC64, 0, 1, -1, 0.0f, 1.0f, 0.0f); }
            virtual void _SeCountDown() { CGameSnd::Play(0xC65, 0, 1, -1, 0.0f, 1.0f, 0.0f); }
            virtual void _SeCountUpDisp() { CGameSnd::Play(0xC0B, 0, 1, -1, 0.0f, 1.0f, 0.0f); }
            // Called by CreateInstance on the new handler, and by the
            // destructor.
            virtual void _Init() {}
            virtual void _Destory() {}
            virtual void _GamePeriodInit(long);
            virtual void _GameInit(long);

            // Decides whether the match is over; `n` is kept at 0x3B.
            void _CheckEndMessage(int n);

            // Set up from the missions of the match (DOL; what they set is
            // not mapped yet).
            void _SetupPanel();
            void _SetupItemParams();
            // Whether player nPlayer's character is on the mission's team.
            int _IsMissionTeam(long nPlayer);
            // The count of the first mission of type nType, 0 without one.
            long _GetCount(long nType) const;
            // On every mission of type nType: set the count to nValue, add
            // nValue to it, or update it from nValue against the target
            // (_CheckPoint; the exact rule is not mapped). Each stores fTime
            // in the mission; a nonzero bSe lets a change count towards the
            // sound played by Main.
            void _SetCount(long nType, long nValue, int bSe, float fTime);
            void _CountUp(long nType, long nValue, int bSe, float fTime);
            void _CheckPoint(long nType, long nValue, int bSe, float fTime);

            // The battle result kept in the tournament's data. Neither takes
            // an object. (return type of the second guessed: whether the
            // stored value is 1)
            // (added for TourF CTournamentStep; static like the two below,
            // return type guessed) Whether the match's mission was cleared.
            static int _IsMissionClear();
            static void _SetBattleWin(long nResult);
            static int _IsBattleWin();

            // Returns the handler there already is; without one, creates a
            // plain CTmGameHandler outside a tournament game and returns
            // null in one, where the game module creates its own.
            static CTmGameHandler *CreateInstance(bcs::CTask *pParent);

            static CTmGameHandler *m_pInstance;

        protected:
            long m_Unk34;        // 0x34, zero from the constructor; low three bits are
                                 // CTournament::GetMissionStartMode, the others rule flags
            signed char m_Unk38; // 0x38, set once _CheckEndMessage has a result
            signed char m_Unk39; // 0x39, set by messages; Main counts down in an extra match while it is
            signed char m_Unk3A; // 0x3A, -1 or what _CheckAppeal returned
            signed char m_Unk3B; // 0x3B, the argument of _CheckEndMessage
            short m_Unk3C;       // 0x3C, zero from the constructor
            short m_Unk3E;       // 0x3E, -1 or a byte of the character holding the ball
            long m_Unk40;        // 0x40, Main calls _SeCountUp and clears it when positive
    };
}

#endif /* !MSW_CTMGAMEHANDLER_HPP_ */
