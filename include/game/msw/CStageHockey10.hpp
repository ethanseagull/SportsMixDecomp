#ifndef MSW_CSTAGEHOCKEY10_HPP_
#define MSW_CSTAGEHOCKEY10_HPP_

#include "bcs/New.hpp"
#include "game/msw/CStage10Base.hpp"
#include "game/msw/CStageManHockey.hpp"

namespace msw {

    // Hockey stage 10, the panel stage (HockeyF), made by
    // CStageFactoryHockey (with -1, meaning not known). The same design as
    // BasketF's CStageBasket10. Size 0x67C: CStageManHockey (0x218), then
    // CStage10Base at 0x218, a DOL class that runs the board of panels,
    // then this class's own members from 0x2F0: the model of every panel
    // kind and number.
    //
    // The CStageMan overrides that only pass on to CStage10Base are inline
    // (the module emits them weak, after the out-of-line functions). The
    // CStage10Base overrides are declared in the order of their slots.
    class CStageHockey10 : public CStageManHockey, public CStage10Base {
        public:
            // The bonus telop: the cockpit's points icon over the goal.
            class CTelopDispHockey : public CTelopDispBase {
                public:
                    CTelopDispHockey(bcs::CTask *pParent) : CTelopDispBase(pParent) {}

                    virtual bcs::FVEC3 GetOffsetPos() const;
                    virtual float GetDispFrame() const;
                    virtual void DoDisp(long nTeam, const bcs::FVEC3 *pPos, long nKind);
            };

            // A panel of this stage: scoring plays a sound (the goal side
            // panel, kind 7, is never scored here), the model is scaled by
            // the radius, and the radius follows the size tuning of the
            // kind.
            class CPanelInfo : public CPanelInfoBase {
                public:
                    CPanelInfo(const CStage10Base *pParent, long nType) : CPanelInfoBase(pParent, nType) {}
                    virtual ~CPanelInfo();

                    virtual bool SetScored();
                    virtual void CalcRootMtx();
                    virtual float GetParam_Radius() const;
                    virtual CTelopDispBase *NewTelopDisp(bcs::CTask *pParent) const { return new (1) CTelopDispHockey(pParent); }
            };

            // (guessed) A float that starts at 0: the constructor clears
            // the array at 0x664 with the loop of an array of a class with
            // an inline constructor.
            struct TIMER {
                TIMER() : m_fTime(0.0f) {}

                float m_fTime;
            };

            CStageHockey10(bcs::CTask *pParent, long);
            virtual ~CStageHockey10();

            // The board's panel queries, passed on to CStage10Base.
            virtual long GetPanelType(const bcs::FVEC3 &rPos) const { return Stage10_GetPanelType(rPos); }
            virtual long HitPanel(const bcs::FVEC3 &rPos, int n) { return Stage10_HitPanel(rPos, n); }
            virtual long GetPanelBonusType() const { return Stage10_GetPanelBonusType(); }
            virtual long GetPanelBonusRate() const { return Stage10_GetPanelBonusRate(); }
            virtual void SendMessage(long msg, long arg1, long arg2, long arg3);
            virtual void SetColorScale(const bcs::BCS_Color &rColor)
            {
                CStageMan::SetColorScale(rColor);
                Stage10_SetColorScale(rColor);
            }
            virtual void Main_(float frame);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual long SE_GetSlotNum() const;
            virtual bool OnPreCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, nw4r::g3d::ResFile file,
                                        CArn &rArn, long n);
            virtual void OnPostCreateObj(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex, bcs::CScnObj *pObj,
                                         nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void OnSceneEntry();
            virtual void OnPostInitialize();
            virtual void OnGoalIn(long, GOAL_IN_TYPE);
            virtual void OnPeriodInit();
            virtual void OnTurnInit();
            virtual void OnGimmickReset();
            virtual void OnGimmickStart();
            virtual void OnGimmickStop();
            virtual void OnGimmickCancel();
            virtual void OnGimmickPause();
            virtual void OnGimmickResume();
            virtual const void *GetPrmTbl() const;
            virtual long GetCurrentPanelType() const { return Stage10_GetCurrentPanelType(); }
            virtual long GetPanelScore(long score) const { return Stage10_GetPanelScore(score, 0); }
            virtual void OnHockeyGoalResult(int bScored);
            virtual void OnHockeyShoot(long nChr);
            virtual void OnSoundEvent(long event);

            virtual int Stage10_IsGimmickRunning() const;
            virtual float GetPanelParam_Size(long nType) const;
            virtual float GetPanelParam_Cycle(long nType) const;
            virtual float GetPanelParam_Life(long nType) const;
            virtual float GetPanelParam_LifeRand(long nType) const;
            virtual long GetPanelParam_RateBeg(long nType) const;
            virtual long GetPanelParam_RateEnd(long nType) const;
            virtual float GetPanelParam_Speed0(long nType) const;
            virtual float GetPanelParam_Speed1(long nType) const;
            virtual float GetPanelParam_StartTime(long nType) const;
            virtual float GetPanelParam_Interval(long nType) const;
            virtual long GetParam_ArrangeSetNum() const;
            virtual long GetParam_ArrangeSet() const;
            virtual long GetParam_ArrangeOddsTblNum() const;
            virtual float GetParam_PanelRespawnTime() const;
            virtual long GetParam_BonusPanelNum() const;
            virtual float GetParam_BonusStartTime(long n) const;
            // Overrides of CStage10Base (a second base) get new slots at the end of
            // the primary vtable in declaration order; the original's order is
            // NewPanel_, AddBonusPanel_, GetArrangeGroup, GetArrangeOddsTbl.
            virtual CPanelInfoBase *NewPanel_(long nType) const;
            virtual void AddBonusPanel_();
            virtual const PANEL_ARRANGE_GROUP *GetArrangeGroup(long nSet, long nGroup) const;
            virtual const ARRANGE_ODDS *GetArrangeOddsTbl(long n) const;

            // The board of the running stage (CStageMan's instance, taken
            // to be this stage; used by CComManHockeyGame).
            static long GetPanelMaxNum();
            static CPanelInfoBase *GetNextPanel(const CPanelInfoBase *pPanel);
            static int IsNowFeverTime();

            // The arrangements of the panels (three sets of five groups)
            // and the odds of the panel kinds by score difference.
            static const PANEL_ARRANGE_GROUP s_PanelArrangeSet[3][5];
            static const ARRANGE_ODDS s_ArrangeOddsTbl[3];

        private:
            // The model of each panel kind (first index) and number (second
            // index; number 1 stands in for a missing one).
            PANEL_MDL_TMPL m_PanelMdl[10][11]; // 0x2F0
            unsigned long m_Unk660;            // 0x660, not touched here
            TIMER m_aTimer[5];                 // 0x664 (purpose not known)
            unsigned long m_Unk678;            // 0x678, not touched here
    };
}

#endif /* !MSW_CSTAGEHOCKEY10_HPP_ */
