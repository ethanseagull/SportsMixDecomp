#include "game/msw/CStage10Base.hpp"
#include "game/msw/CCockpit.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/BCS_Vec.hpp"

namespace msw {

    CStage10Base::CTelopDispBase::CTelopDispBase(bcs::CTask *pParent)
        : bcs::CUnivTaskEx(pParent, -1),
          m_bActive(0),
          m_fTime(90.0f)
    {
    }

    CStage10Base::CTelopDispBase::~CTelopDispBase()
    {
        Erase();
    }

    void CStage10Base::CTelopDispBase::Init(long nTeam, const CPanelInfoBase *pPanel)
    {
        long nGoal = CGame::GetTargetGoalIdx(nTeam & 1);
        bcs::FVEC3 offset = GetDispOffset();
        bcs::FVEC3 pos = CGame::GetGoalPos(nGoal);
        long nKind = FindKind(pPanel);

        bcs::FVEC3Add(&pos, &pos, &offset);
        Open(nTeam, pos, nKind);
        m_fTime = GetDispTime();
        m_bActive = 1;
    }

    void CStage10Base::CTelopDispBase::Main()
    {
        if (IsDelete()) {
            return;
        }
        if (m_bActive == 0) {
            return;
        }

        float rate = GetFrameRateF();
        if (rate > 0.0f && m_fTime >= 0.0f) {
            m_fTime -= rate;
            if (m_fTime <= 0.0f) {
                Delete();
            }
        }
    }

    void CStage10Base::CTelopDispBase::Erase()
    {
        CCockpit::CloseTelopMiniPos(1);
    }

    bcs::FVEC3 CStage10Base::CTelopDispBase::GetDispOffset() const
    {
        return bcs::FVEC3(0.0f, 1.0f, 0.0f);
    }

    long CStage10Base::CTelopDispBase::FindKind(const CPanelInfoBase *pPanel) const
    {
        long nKind = -1;
        long nType = pPanel->GetPanelBonusType();
        long nRate = pPanel->GetPanelBonusRate();

        switch (nType) {
            case 1:
                switch (nRate) {
                    case 5:
                        nKind = 0;
                        break;
                    case 10:
                        nKind = 1;
                        break;
                    case 15:
                        nKind = 2;
                        break;
                    case 20:
                        nKind = 3;
                        break;
                    case 30:
                        nKind = 4;
                        break;
                    case 0:
                        nKind = 5;
                        break;
                    case -5:
                        nKind = 6;
                        break;
                    case -10:
                        nKind = 7;
                        break;
                    case -15:
                        nKind = 8;
                        break;
                    case -20:
                        nKind = 9;
                        break;
                }
                break;
            case 2:
                switch (nRate) {
                    case 1:
                        nKind = 10;
                        break;
                    case 2:
                        nKind = 11;
                        break;
                    case 3:
                        nKind = 12;
                        break;
                    case 4:
                        nKind = 13;
                        break;
                    case 5:
                        nKind = 14;
                        break;
                    case 10:
                        nKind = 15;
                        break;
                }
                break;
        }
        return nKind;
    }
}
