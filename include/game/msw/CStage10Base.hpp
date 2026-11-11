#ifndef MSW_CSTAGE10BASE_HPP_
#define MSW_CSTAGE10BASE_HPP_

#include "bcs/CListNodeBase.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgSrt.hpp"

namespace bcs {
    class CScnObj;
    struct BCS_Color;
}

namespace msw {

    class CArn;

    // (purpose guessed from the names) Stage 10, the panel stage: the board
    // of score and bonus panels the players run over. A DOL class used as a
    // second base by the sports' stages (BasketF's CStageBasket10, which has
    // it at 0x218). Size 0xD8: data first, the vptr at 0xD4 (DOL constructor
    // __ct__Q23msw12CStage10BaseFv).
    //
    // The virtual functions follow the DOL vtable (lbl_80472D0C) and the
    // second part of __vt__Q23msw14CStageBasket10. The bodies given here are
    // the ones BasketF emits as weak functions; the slots the DOL leaves
    // null are pure. Names come from the symbols; return types not visible
    // in a body, parameter names and member names are guesses.
    class CStage10Base {
        public:
            class CTelopDispBase;

            // (names guessed) Where the model of a panel kind comes from:
            // the archive and the index of the node in it.
            // CStageBasket10::OnPreCreateObj fills them, NewPanel_ hands one
            // to the new panel; no archive is an index of -1.
            struct PANEL_MDL_TMPL {
                PANEL_MDL_TMPL() : m_pArn(0), m_nIdx(-1) {}

                CArn *m_pArn; // 0x00
                long m_nIdx;  // 0x04
            };

            // (names guessed) One panel of an arrangement: its kind (-1 ends
            // the list), its place on the court and how far it moves.
            struct PANEL_ARRANGE {
                long m_nType;       // 0x00
                float m_fX;         // 0x04
                float m_fZ;         // 0x08
                float m_fMoveWidth; // 0x0C
            };

            // (names guessed) A group of panels placed together, 0x84 bytes.
            struct PANEL_ARRANGE_GROUP {
                long m_nUnk00;                  // 0x00
                PANEL_ARRANGE m_aPanel[8];      // 0x04
            };

            // (names guessed) The odds of the panel kinds while the score
            // difference is at most m_nScoreDiff (SelectPanel_ walks the
            // tables in order); a kind of 0 ends the list. 0x64 bytes.
            struct ARRANGE_ODDS {
                struct ODDS {
                    long m_nType;    // 0x00
                    long m_nSubType; // 0x04, put in the upper half of the panel's type
                    long m_nOdds;    // 0x08
                };

                long m_nScoreDiff; // 0x00
                ODDS m_aOdds[8];   // 0x04
            };

            // One panel of the board, 0x1A4 bytes. A DOL class, a list node
            // (the panels are in the list at CStage10Base's 0x00). Its type
            // holds the kind in the lower 16 bits (7: the goal side panel,
            // 9: the bonus panel) and a sub kind in the upper 16.
            class CPanelInfoBase : public bcs::CListNodeBase {
                public:
                    enum STATE_EVENT {
                    };

                    CPanelInfoBase(const CStage10Base *pParent, long nType);

                    // The name HockeyF's CComManHockeyGame uses for
                    // IsActiveScorePanel (written before the slot's name
                    // was known).
                    int IsEnable() const { return IsActiveScorePanel(); }

                private:
                    const CStage10Base *m_pParent; // 0x10
                    long m_nType;                  // 0x14
                    bcs::CScnObj *m_pMdl;          // 0x18 (type guessed)
                    float m_fRadius;               // 0x1C
                    CStgSrt m_Srt;                 // 0x20
                    bcs::FVEC3 m_InitPos;          // 0x164
                    float m_fMoveWidth;            // 0x170
                    long m_nState;                 // 0x174, 2 locked, 3 scored, 6 terminated
                    long m_nNextState;             // 0x178
                    unsigned char m_Unk17C[0x8];   // 0x17C
                    float m_fLife;                 // 0x184
                    float m_fLifeMax;              // 0x188
                    unsigned char m_Unk18C[0xC];   // 0x18C
                    long m_nTag;                   // 0x198
                    unsigned char m_Unk19C[0x8];   // 0x19C

                public:
                    virtual ~CPanelInfoBase();                                           // 0x08
                    virtual void Create(const PANEL_MDL_TMPL &rTmpl);                    // 0x1C
                    virtual void Main(float frame);                                      // 0x20
                    virtual const bcs::FVEC3 &GetPos() const { return m_Srt.GetPos(); }  // 0x24
                    virtual void SetPos(const bcs::FVEC3 &rPos) { m_Srt.SetPos(rPos); }  // 0x28
                    virtual void SetInitPos(const bcs::FVEC3 &rPos)                      // 0x2C
                    {
                        m_InitPos = rPos;
                        SetPos(rPos);
                    }
                    virtual float GetMoveWidth() const { return m_fMoveWidth; }          // 0x30
                    virtual void SetMoveWidth(float width) { m_fMoveWidth = width; }     // 0x34
                    virtual long GetTag() const { return m_nTag; }                       // 0x38
                    virtual void SetTag(long tag) { m_nTag = tag; }                      // 0x3C
                    virtual long GetType() const { return m_nType; }                     // 0x40
                    virtual long GetState() const { return m_nState; }                   // 0x44
                    virtual float GetRadius() const { return m_fRadius; }                // 0x48
                    virtual bool IsRide(const bcs::FVEC3 &rPos) const;                   // 0x4C
                    virtual bool IsRideAnyChr() const;                                   // 0x50
                    virtual bool IsOverlap(const CPanelInfoBase *pPanel) const;          // 0x54
                    // Sets the state the panel goes to next.
                    virtual void SetState(long state) { m_nNextState = state; }          // 0x58
                    virtual void Lock();                                                 // 0x5C
                    // Locked, or the bonus panel.
                    virtual int IsLocked() const                                         // 0x60
                    {
                        int locked = 1;
                        if (m_nState != 2 && (GetType() & 0xFFFF) != 9) {
                            locked = 0;
                        }
                        return locked;
                    }
                    virtual void SetScored();                                            // 0x64
                    virtual int IsScored() const { return m_nState == 3; }               // 0x68
                    virtual void SetFailed();                                            // 0x6C
                    virtual void Terminate();                                            // 0x70
                    virtual int IsTerminated() const { return m_nState == 6; }           // 0x74
                    virtual long GetPanelScore(long score) const;                        // 0x78
                    // (return types guessed) The bonus kind (1 and 2 seen)
                    // and its rate; CTelopDispBase::FindKind maps the pair
                    // to a telop.
                    virtual long GetPanelBonusType() const;                              // 0x7C
                    virtual long GetPanelBonusRate() const;                              // 0x80
                    // Shows the rate over character nChr's head.
                    virtual void RateDisp(long nChr);                                    // 0x84
                    virtual void RateDispErase();                                        // 0x88
                    virtual void ResetRot();                                             // 0x8C
                    // Whether the panel is in play (HockeyF's
                    // CComManHockeyGame skips the others; int as its calls
                    // show).
                    virtual int IsActiveScorePanel() const;                              // 0x90
                    virtual int IsActiveBonusPanel() const;                              // 0x94
                    virtual CStgSrt &GetSrt() { return m_Srt; }                          // 0x98
                    virtual const CStgSrt &GetSrt() const { return m_Srt; }              // 0x9C
                    virtual bcs::CScnObj *GetMdl() { return m_pMdl; }                    // 0xA0
                    virtual const bcs::CScnObj *GetMdl() const { return m_pMdl; }        // 0xA4
                    virtual const CStage10Base *GetParent() const { return m_pParent; }  // 0xA8
                    virtual void Init();                                                 // 0xAC
                    virtual void CalcRootMtx();                                          // 0xB0
                    virtual void Proc(STATE_EVENT event, float frame);                   // 0xB4
                    virtual void CheckState();                                           // 0xB8
                    virtual void MoveUpdate(float frame);                                // 0xBC
                    virtual float GetLife() const { return m_fLife; }                    // 0xC0
                    virtual float GetLifeMax() const { return m_fLifeMax; }              // 0xC4
                    // The radius of the panel, from the size of its kind.
                    virtual float GetParam_Radius() const = 0;                           // 0xC8
                    virtual void PlayAnim(long, int, float, float, float);               // 0xCC
                    virtual void PushAnim(long, int, float, float, float);               // 0xD0
                    virtual void PlayLoopAnim(long, float, float);                       // 0xD4
                    virtual bool IsAnimEnd() const;                                      // 0xD8
                    virtual bool IsAnimEndAll() const;                                   // 0xDC
                    virtual void SetSubType(long subType);                               // 0xE0
                    virtual void PlaySe(long se);                                        // 0xE4
                    virtual void StopSe();                                               // 0xE8
                    virtual bool IsPlaySe() const;                                       // 0xEC
                    // The telop shown when the panel is scored.
                    virtual CTelopDispBase *NewTelopDisp(bcs::CTask *pParent) const; // 0xF0
                    // 0xF4: null in the DOL's vtable and in BasketF's
                    // CStageBasket10::CPanelInfo, which is made with new, so it
                    // cannot have been pure. Declared and never defined (the
                    // original's linkers left the slot null; ours needs a
                    // definition to link). Name and type not known.
                    virtual void UnkF4();
            };

            // (purpose guessed) A telop shown over a goal when a bonus
            // panel is scored; it removes itself when its time runs out.
            // A DOL class; the derived classes give the display (DoDisp).
            // The names of the offset, the time and the display are those
            // of BasketF's overrides (CStageBasket10::CTelopDispBasket).
            class CTelopDispBase : public bcs::CUnivTaskEx {
                public:
                    CTelopDispBase(bcs::CTask *pParent);
                    virtual ~CTelopDispBase();
                    virtual void Main();
                    // Starts the telop for team nTeam's target goal with the
                    // telop kind of the panel.
                    virtual void Init(long nTeam, const CPanelInfoBase *pPanel);
                    virtual void Erase();
                    // Where the telop is shown, relative to the goal.
                    virtual bcs::FVEC3 GetOffsetPos() const;
                    // How long the telop stays (frames).
                    virtual float GetDispFrame() const { return 90.0f; }
                    // The telop kind for a panel's bonus; -1 for none.
                    virtual long FindKind(const CPanelInfoBase *pPanel) const;
                    // Shows telop nKind for team nTeam at *pPos. Pure (null
                    // in the DOL's vtable).
                    virtual void DoDisp(long nTeam, const bcs::FVEC3 *pPos, long nKind) = 0;

                private:
                    int m_bActive;     // 0x34 (name guessed) set by Init
                    float m_fTime;     // 0x38 (name guessed) frames left
            };

            CStage10Base();

            // The panel after pPanel in the board's list (the first one
            // for null), or null after the last one.
            CPanelInfoBase *GetNextPanel_(const CPanelInfoBase *pPanel) const;

        private:
            bcs::CListBase m_PanelList;       // 0x00, the panels (a bcs::CList with its own vtable)
            float m_Unk28;                    // 0x28
            PANEL_MDL_TMPL m_FeverMdl[10];    // 0x2C (name guessed; see CreateFeverMdl_)
            float m_Unk7C;                    // 0x7C
            long m_nBoardState;               // 0x80
            unsigned long m_Unk84;            // 0x84
            unsigned long m_Unk88;            // 0x88
            unsigned long m_Unk8C;            // 0x8C
            float m_Unk90;                    // 0x90
            float m_Unk94;                    // 0x94
            unsigned long m_Unk98;            // 0x98
            unsigned long m_Unk9C;            // 0x9C
            long m_nBonusType;                // 0xA0
            long m_nBonusRate;                // 0xA4
            unsigned long m_UnkA8;            // 0xA8
            float m_UnkAC[5];                 // 0xAC
            unsigned long m_UnkC0;            // 0xC0
            int m_bExRule;                    // 0xC4
            unsigned char m_SeSlot[0xC];      // 0xC8, a msw::CStgSeSlot subclass (vptr at 0xD0)

        public:
            virtual ~CStage10Base();                                                        // 0x08
            virtual long Stage10_GetPanelType(const bcs::FVEC3 &rPos) const;                // 0x0C
            virtual long Stage10_HitPanel(const bcs::FVEC3 &rPos, int);                     // 0x10
            virtual long Stage10_GetPanelBonusType() const { return m_nBonusType; }         // 0x14
            virtual long Stage10_GetPanelBonusRate() const { return m_nBonusRate; }         // 0x18
            virtual int Stage10_IsGimmickRunning() const = 0;                               // 0x1C
            virtual long Stage10_GetCurrentPanelType() const;                               // 0x20
            virtual long Stage10_GetPanelScore(long score, long) const;                     // 0x24

            // Tuning of a panel kind (the lower 16 bits of nType), given by
            // the sport's stage.
            virtual float GetPanelParam_Size(long nType) const = 0;                         // 0x28
            virtual float GetPanelParam_Cycle(long nType) const = 0;                        // 0x2C
            virtual float GetPanelParam_Life(long nType) const = 0;                         // 0x30
            virtual float GetPanelParam_LifeRand(long nType) const = 0;                     // 0x34
            virtual long GetPanelParam_RateBeg(long nType) const = 0;                       // 0x38
            virtual long GetPanelParam_RateEnd(long nType) const = 0;                       // 0x3C
            virtual float GetPanelParam_Speed0(long nType) const = 0;                       // 0x40
            virtual float GetPanelParam_Speed1(long nType) const = 0;                       // 0x44
            virtual float GetPanelParam_StartTime(long nType) const = 0;                    // 0x48
            virtual float GetPanelParam_Interval(long nType) const = 0;                     // 0x4C
            // Tuning of the board.
            virtual long GetParam_ArrangeSetNum() const = 0;                                // 0x50
            virtual long GetParam_ArrangeSet() const = 0;                                   // 0x54
            virtual long GetParam_ArrangeOddsTblNum() const = 0;                            // 0x58
            virtual float GetParam_PanelRespawnTime() const = 0;                            // 0x5C
            virtual long GetParam_ArrangeGroupNum() const { return 5; }                     // 0x60
            virtual long GetParam_BonusPanelNum() const = 0;                                // 0x64
            virtual long GetParam_FeverSwapTimer() const { return 0; }                      // 0x68
            virtual float GetParam_BonusStartTime(long n) const;                            // 0x6C

            virtual void Stage10_Main_(float frame);                                        // 0x70
            virtual void InitPanelAll();                                                    // 0x74
            virtual void DeletePanelAll();                                                  // 0x78
            virtual void ResetPanelAll();                                                   // 0x7C
            virtual void Stage10_SetColorScale(const bcs::BCS_Color &rColor);               // 0x80
            virtual void SetExRule(int bExRule) { m_bExRule = bExRule; }                    // 0x84
            virtual int IsExRule() const { return m_bExRule; }                              // 0x88
            virtual int IsPanelArrangeReady_() const;                                       // 0x8C
            virtual void PanelArrangeUpdate_(float frame);                                  // 0x90
            virtual void PanelArrangeUpdateSub_(long, float frame);                         // 0x94
            virtual void PanelUpdate_(float frame);                                         // 0x98
            virtual void ResetPanelRotAll();                                                // 0x9C
            virtual const PANEL_ARRANGE_GROUP *GetArrangeGroup(long nSet, long nGroup) const = 0; // 0xA0
            virtual const ARRANGE_ODDS *GetArrangeOddsTbl(long n) const = 0;                // 0xA4
            virtual void BoardUpdate_(float frame);                                         // 0xA8
            virtual long GetBoardState() const { return m_nBoardState; }                    // 0xAC
            virtual void CreateFeverMdl_(CArn &rArn, long n);                               // 0xB0
            virtual void FeverStart_(long);                                                 // 0xB4
            virtual void FeverUpdate_(float frame);                                         // 0xB8
            virtual void FeverCancel_();                                                    // 0xBC
            virtual int IsNowFeverTime_() const;                                            // 0xC0

            // Hooks the sport's stage calls from its own.
            virtual void Stage10_OnPostInitialize();                                        // 0xC4
            virtual void Stage10_OnTurnInit();                                              // 0xC8
            virtual void Stage10_OnPeriodInit();                                            // 0xCC
            virtual void Stage10_OnGimmickReset();                                          // 0xD0
            virtual void Stage10_OnGimmickStart();                                          // 0xD4
            virtual void Stage10_OnGimmickStop();                                           // 0xD8
            virtual void Stage10_OnGimmickCancel();                                         // 0xDC
            virtual void Stage10_OnGimmickPause();                                          // 0xE0
            virtual void Stage10_OnGimmickResume();                                         // 0xE4

            virtual long GetPanelNum_() const { return m_PanelList.GetCount(); }            // 0xE8
            virtual long GetPanelNumByGroup_(long nGroup) const;                            // 0xEC
            // Makes a panel of type nType (kind in the lower 16 bits).
            virtual CPanelInfoBase *NewPanel_(long nType) const = 0;                        // 0xF0
            virtual void AddPanel_(long nType);                                             // 0xF4
            virtual void AddPanelAuto_(long, int);                                          // 0xF8
            virtual void AddBonusPanel_();                                                  // 0xFC
            virtual void AddBonusDisp_();                                                   // 0x100
            virtual long SelectPanel_(long, long, int);                                     // 0x104
            virtual CPanelInfoBase *FindPanel_(const bcs::FVEC3 &rPos, CPanelInfoBase *pFrom);             // 0x108
            virtual const CPanelInfoBase *FindPanel_(const bcs::FVEC3 &rPos, const CPanelInfoBase *pFrom) const; // 0x10C
            // Starts the panel under rPos.
            virtual void TriggerPanel_(const bcs::FVEC3 &rPos);                             // 0x110
            // The panel being played for, or null.
            virtual CPanelInfoBase *GetActivePanel_();                                      // 0x114
            virtual const CPanelInfoBase *GetActivePanel_() const;                          // 0x118
            virtual CPanelInfoBase *FindPanelByTag_(long tag);                              // 0x11C
            virtual void CleanupPanel_();                                                   // 0x120
            virtual bool PanelOverlapTest_(const CPanelInfoBase *pPanel) const;             // 0x124
            virtual void AnimSync_(float frame);                                            // 0x128
    };
}

#endif /* !MSW_CSTAGE10BASE_HPP_ */
