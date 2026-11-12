#ifndef MSW_CGAMEGUIDEMAN_HPP_
#define MSW_CGAMEGUIDEMAN_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CChrCore;

    // (name guessed) The guide telop task CGameGuideMan makes as its child,
    // size 0x60 (operator new in CGameGuideMan's constructor, DOL
    // fn_80269964). Its functions follow CGameGuideMan's in the DOL.
    class CGameGuideTelop : public bcs::CTask {
        public:
            // (DOL fn_8026AE24; name, argument meanings and return type
            // guessed) Shows guide telop nType for the character with the
            // given player, team and controlling player numbers; not zero
            // when a new telop was put up.
            int Start(long nType, long nPlayer, long nTeam, long nCtrlPlayer, const bcs::FVEC3 *pOfs, int bPad,
                      float fTime, float, float, float);

            // (name guessed) Takes the telop down and forgets it.
            // (name guessed) Takes the telop down and forgets it. Inline,
            // defined in game/msw/CGameGuideMan.cpp.
            inline void Remove();

            // (names guessed)
            float m_fTime;              // 0x34
            long m_nType;               // 0x38, -1 when no telop is up
            long m_nTelopId;            // 0x3C, -1 when none
            long m_nPlayer;             // 0x40
            int m_bEnd;                 // 0x44, asks the telop to end
            int m_bGameTelop;           // 0x48, made through CGame::TelopCreate
            unsigned long m_nTaskId;    // 0x4C, a task looked up through CTaskMan
            unsigned char m_Unk50[0x60 - 0x50];
    };

    // The in-game guide (help telops and line messages over the
    // characters). A DOL task of 0x238 bytes, one instance at a time; the
    // static functions call the virtual ones (named with a trailing
    // underscore) on it. Names of the virtuals, parameter meanings and
    // return types are guesses unless said otherwise.
    class CGameGuideMan : public bcs::CTask {
        public:
            CGameGuideMan(bcs::CTask *pParent);
            virtual ~CGameGuideMan();
            virtual void Main();
            virtual void Disp();
            virtual void Reset_();                                         // 0x18
            virtual int SendMessage_(long nMsg, long, long, long);         // 0x1C
            virtual void MainPre_();                                       // 0x20
            virtual void MainGuide_(float fRate);                          // 0x24
            virtual void MainPlayer_(float fRate);                         // 0x28
            virtual int IsGuideDisp_(long nType, long nPlayer);            // 0x2C
            virtual long GetGuideCount_(long nType, long nPlayer);         // 0x30
            virtual void AddGuideCount_(long nType, long nPlayer);         // 0x34
            virtual long GetGuideSuccessCount_(long nType, long nPlayer);  // 0x38
            virtual long AddGuideSuccessCount_(long nType, long nPlayer);  // 0x3C
            // nPlayer < 0 clears the count of every player.
            virtual void ClearGuideCount_(long nType, long nPlayer);       // 0x40
            virtual void ClearGuideSuccessCount_(long nType, long nPlayer); // 0x44
            virtual int StartGuideTelop_(long nType, CChrCore *pChr, float fTime, const bcs::FVEC3 *pOfs, float,
                                         float, float);                    // 0x48
            virtual void EndGuideTelop_(long nPlayer);                     // 0x4C
            virtual void RemoveGuideTelop_();                              // 0x50
            virtual int StartLineMsg_(long nType, CChrCore *pChr, int, float); // 0x54
            virtual int IsLineMsgDisp_(long nType);                        // 0x58
            virtual void EndLineMsg_(int);                                 // 0x5C
            virtual void StartPlayerGuide_(long nPlayer, int, float);      // 0x60
            virtual void EndPlayerGuide_(long nPlayer, int);               // 0x64
            virtual void EndAllPlayerGuide_(int);                          // 0x68
            virtual int IsGuideEnable_();                                  // 0x6C

            // Not zero while an instance exists.
            static int IsActive();
            // (DOL fn_80269E18) Makes the instance under pParent when the
            // game allows guides; gives the instance (null without one).
            static CGameGuideMan *Create(bcs::CTask *pParent);
            // (DOL fn_80269E9C, fn_80269FC8) Reset_ and SendMessage_ on the
            // instance.
            static void Reset();
            static int SendMessage(long nMsg, long, long, long);
            static long GetGuideSuccessCount(long nType, long nPlayer);
            static long AddGuideSuccessCount(long nType, long nPlayer);
            static int IsGuideDisp(long nType, long nPlayer);
            // Not zero while guides may be shown.
            static int IsDispEnable();
            // Shows guide telop nType for pChr for fTime frames, offset by
            // *pOfs. The last three numbers are not understood (-1 when not
            // set).
            static int StartGuideTelop(long nType, CChrCore *pChr, float fTime, const bcs::FVEC3 *pOfs, float, float, float);
            static void EndGuideTelop(long nType);
            static void RemoveGuideTelop();
            static int IsLineMsgDisp(long nType);
            // Shows line message nType over pChr.
            static int StartLineMsg(long nType, CChrCore *pChr, int, float);
            static void EndLineMsg(int);

            static CGameGuideMan *s_pInstance;

            // (names guessed)
            bcs::CTask *m_pChildRoot;          // 0x34
            CGameGuideTelop *m_pTelop;         // 0x38
            int m_bLineMsg;                    // 0x3C
            long m_nLineMsgType;               // 0x40, -1 when none
            float m_fLineMsgTime;              // 0x44
            int m_bPlayerGuide[4];             // 0x48
            float m_fPlayerGuideTime[4];       // 0x58
            short m_GuideCount[4][29];         // 0x68
            short m_GuideSuccessCount[4][29];  // 0x150
    };
}

#endif /* !MSW_CGAMEGUIDEMAN_HPP_ */
