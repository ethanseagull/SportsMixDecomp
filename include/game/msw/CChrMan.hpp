#ifndef MSW_CCHRMAN_HPP_
#define MSW_CCHRMAN_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrDrv.hpp"

namespace msw {

    class CBall;
    class CChrCore;
    class CNpcBase;

    // Owner of the characters on the court. A DOL class and a task: its
    // constructor calls bcs::CTask's with priority 0x14 and its vtable has
    // bcs::CUnivTask's GetFrameRateF.
    //
    // Size 0x8C; each game module derives its own manager (CChrManBoss,
    // CChrManBasket, ...).
    //
    // The virtual functions are in the order of the DOL's vtable
    // (lbl_8046E2C0), named from BossF's __vt__Q23msw11CChrManBoss. Create_
    // and the Util_ slots 0xC0 to 0xE0 are zero in the DOL's vtable: pure.
    // The ones with a body are those the modules emit as weak functions
    // (BossF after CChrManBoss's code). Return types are guesses unless a
    // body is given.
    //
    // INCOMPLETE: only some members are named.
    class CChrMan : public bcs::CUnivTask {
        public:
            // nChrMax: the size of the character tables (the constructor
            // allocates m_ppChrCore and the table at 0x58 with it).
            CChrMan(bcs::CTask *pParent, long nChrMax);
            virtual ~CChrMan();
            virtual void Main();
            virtual void Disp();

            virtual void ResetShootMarker(long) {}                                       // 0x18
            virtual void CalcSubCharPos_() {}                                            // 0x1C
            virtual void ChrCoreMainCall_();                                             // 0x20
            virtual void CheckPassMode_(CChrCore *) {}                                   // 0x24
            virtual void CheckChgLeaderOrder_() {}                                       // 0x28
            virtual void MainFunc_() {}                                                  // 0x2C
            virtual void DispFunc() {}                                                   // 0x30
            virtual void Create_() = 0;                                                  // 0x34
            virtual void SetPadIdAssign_(long nNum, long *pId);                          // 0x38
            virtual void SetCamIdAssign_(long nNum, long *pId);                          // 0x3C
            virtual void SetDrvIdAssign_(long nNum, long *pId);                          // 0x40
            virtual long GetAssignPadNo_(long);                                          // 0x44
            virtual int IsNeedSubComChrMarker_(long);                                    // 0x48
            virtual void AllReset_();                                                    // 0x4C
            virtual void InitLeader_(int);                                               // 0x50
            virtual void SendMsg_(long, CChrDrv::MSG_ID, long, long, unsigned long, unsigned long); // 0x54
            virtual void AllSendMsg_(CChrDrv::MSG_ID, long, long, unsigned long, unsigned long);   // 0x58
            virtual void AllSetState_(long, long, long);                                 // 0x5C
            virtual void ReserveAllSetState_(long, long, long);                          // 0x60
            virtual int CanbeTurnOver_();                                                // 0x64
            virtual int CanbeScoredView_();                                              // 0x68
            virtual void DecideScoredViewLookChr_(long);                                 // 0x6C
            virtual CChrCore *GetScoredViewLookChr_(long);                               // 0x70
            virtual void StartScoreView_(long);                                          // 0x74
            virtual void AllStop_(CChrCore::STOP_MODE, CChrCore::STOP_MODE);             // 0x78
            virtual void StartArtsEvent_(long);                                          // 0x7C
            virtual void SendMessage_(long, long, long, long);                           // 0x80
            virtual long GetInnerChrNum_(long);                                          // 0x84
            virtual long GetOuterChrNum_(long);                                          // 0x88
            virtual CChrCore *Util_SearchNearPos_(const bcs::FVEC3 &, long, int, CChrCore *); // 0x8C
            virtual CChrCore *Util_SearchNearBall_(long, long, int, CChrCore *);         // 0x90
            virtual CChrCore *Util_SearchNearBall_(const CBall *, long, int, CChrCore *); // 0x94
            virtual CChrCore *Util_SearchNearChar_(long, long, int, CChrCore *);         // 0x98
            virtual CChrCore *Util_SearchNearPosEnemy_(long);                            // 0x9C
            virtual CChrCore *Util_SearchNearRotEnemy_(long, float);                     // 0xA0
            virtual CChrCore *Util_SearchNearPosChar_(const bcs::FVEC3 &, long);         // 0xA4
            virtual CChrCore *Util_SearchNearRotChar_(const bcs::FVEC3 &, float, long);  // 0xA8
            virtual CChrCore *Util_SearchHaveBallChr_(long);                             // 0xAC
            virtual CChrCore *Util_SearchHoldBallChr_(long);                             // 0xB0
            virtual CNpcBase *Util_SearchHaveBallNpc_(long);                             // 0xB4
            virtual CNpcBase *Util_SearchHoldBallNpc_(long);                             // 0xB8
            virtual int Util_GetMyTeamFlag_(long);                                       // 0xBC
            virtual CChrCore *Util_SearchChgLeader_(long, long, CChrCore *) = 0;         // 0xC0
            virtual CChrCore *Util_SearchNearOwnGoal_(long, int, CChrCore *) = 0;        // 0xC4
            virtual CChrCore *Util_SearchPassTo_(long nChr, float) = 0;                  // 0xC8
            virtual CChrCore *Util_SearchPassmeetTo_(long nChr, float, bcs::FVEC3 &rPos) = 0; // 0xCC
            virtual CChrCore *Util_SearchArtsPassTo_(long nChr) = 0;                     // 0xD0
            virtual int Util_GetSubChrTargetPos_(long nChr, bcs::FVEC3 &rPos) = 0;       // 0xD4
            virtual CChrCore *Util_GetSubChrMarkChrCore_(CChrCore *) = 0;                // 0xD8
            virtual void Util_SetChgLeaderOrder_(long) = 0;                              // 0xDC
            virtual long Util_GetNextChgLeader_(long) = 0;                               // 0xE0
            virtual int Is1on1CameraDirection_();                                        // 0xE4

            // DOL. Makes character nChr the leader of team nTeam.
            // (parameter meanings guessed; Create_ and InitLeader_ pass
            // (i, i, 0) for teams 0 and 1)
            void SetTeamLeader(long nTeam, long nChr, int);
            // DOL. Character nChr of this manager.
            CChrCore *GetChrCore_(long nChr);

            // DOL, static: the instance as stored, and its setter.
            static void SetInstance_(CChrMan *pInstance);
            static CChrMan *GetInstance_();

            // The character with index `nChr` (0 to GetChrCoreNum() - 1), or
            // null when there is no manager or the index is out of range.
            // Static: it finds the instance itself.
            static CChrCore *GetChrCore(long nChr);
            // The number of characters: the word at 0x50 of the instance,
            // zero when there is none. Static, like GetChrCore.
            static long GetChrCoreNum();
            // Shows or hides the markers over the characters. Static.
            static void SetChrMarkerDisp(int bDisp);
            // DOL. The instance, or null.
            static CChrMan *GetInstance();
            // DOL, static. Not zero when the instance exists.
            static int IsActive();
            // DOL, static. The character of team nTeam that has the ball,
            // or null. (BasketF's CStageBasket09 passes 0)
            static CChrCore *Util_SearchHaveBallChr(long nTeam);
            // DOL, static. The next NPC of kind `nKind` after pPrev (the
            // first with a null pPrev), or null. (parameter meanings guessed
            // from the callers: BasketF's CChrManBasket::GetBossPackun_ walks
            // the NPCs of kind 2 this way)
            static CNpcBase *NpcSearch(long nKind, CNpcBase *pPrev);
            // (name guessed; fn_800EC2E4, unnamed in the DOL) Not zero when
            // a character that is not disabled has bit 0x20000 of the word
            // at 0xE8 set (taken to be the star state). Static.
            static int IsExistStarChr();
            // (DOL, static; return type and argument meanings not known)
            // Passes a message to the NPCs. BasketF's CBallBasketFruit sends
            // (0xB, 0, -1, ball number, 0) when a fruit on a string explodes.
            static void NpcSendMessage(long nMsg, long, long, long, long);

        protected:
            unsigned char m_Unk34[0x50 - 0x34]; // two nw4r::ut::List at 0x34 and 0x40, a word at 0x4C
            long m_nChrCoreNum;                 // 0x50, the number of characters (GetChrCoreNum)
            CChrCore **m_ppChrCore;             // 0x54, the characters (nChrMax entries)
            long *m_Unk58;                      // 0x58, nChrMax words
            // 0x5C (name guessed): a child task of this one; the characters
            // are created under it.
            bcs::CTask *m_pChrTask;
            unsigned long m_Unk60;
            // 0x64, 0x6C (names guessed): ChrCoreMainCall_ runs the
            // characters in this order; each entry starts with the CChrCore.
            long m_nChrOrderNum;
            unsigned long m_Unk68;
            CChrCore ***m_pppChrOrder;
            unsigned char m_Unk70[0x8C - 0x70];
    };
}

#endif /* !MSW_CCHRMAN_HPP_ */
