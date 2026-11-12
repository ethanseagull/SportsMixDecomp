#ifndef CRADARICON2DHOCKEYGOAL_HPP_
#define CRADARICON2DHOCKEYGOAL_HPP_

#include "game/msw/CMenuLinkBase.hpp"
#include "game/msw/CMenuLytObj.hpp"
#include "bcs/G2D/CPaneCtrl.hpp"
#include "bcs/G2D/CPaneObj.hpp"
#include "nw4r/lyt/Pane.hpp"

namespace bcs {
    struct FVEC3;

    namespace G2D {
        class CPaneObj;
    }
}

namespace msw {

    // Size 0x44 (CCockpitHockey::CreateObjImpl allocates it): the radar icons
    // of the goals (HockeyF game/hockey/CRadarIcon2DHockeyGoal.cpp). As
    // CRadarIcon2D: CMenuLinkBase at 0 (vptr at 0x18), CMenuLytObj at 0x1C
    // (vptr at 0x28), own members from 0x2C. Member names are guesses.
    class CRadarIcon2DHockeyGoal : public CMenuLinkBase, public CMenuLytObj<bcs::G2D::CPaneCtrl, 1> {
        public:
            // One goal icon: four panes and a word nothing sets. 0x24 bytes.
            struct ICON_PARAMETER {
                enum {
                    PANE_ROOT,    // empty pane under the layout's centre
                    PANE_NULL,    // empty pane that is moved and turned
                    PANE_BASE,    // frame copied from the layout, by team
                    PANE_ARROW,   // pointer copied from the layout
                    PANE_NUM
                };

                // (name guessed) Inline in InitGoalIcons.
                void Clear()
                {
                    m_pPaneName[PANE_ROOT] = 0;
                    m_pPaneName[PANE_NULL] = 0;
                    m_pPaneName[PANE_BASE] = 0;
                    m_pPaneName[PANE_ARROW] = 0;
                    m_pPaneObj[PANE_ROOT] = 0;
                    m_pPaneObj[PANE_NULL] = 0;
                    m_pPaneObj[PANE_BASE] = 0;
                    m_pPaneObj[PANE_ARROW] = 0;
                    m_Unk20 = 0;
                }

                // (name guessed) As CRadarIcon2D::ICON_PARAMETER's.
                void SetPaneObj(int nPane, bcs::G2D::CPaneObj *pPaneObj)
                {
                    if (pPaneObj != 0) {
                        m_pPaneName[nPane] = pPaneObj->GetPane()->GetName();
                        m_pPaneObj[nPane] = pPaneObj;
                    }
                }

                const char *m_pPaneName[PANE_NUM];        // 0x00
                bcs::G2D::CPaneObj *m_pPaneObj[PANE_NUM]; // 0x10
                unsigned long m_Unk20;                    // 0x20, only cleared
            };

            enum {
                VIEW_MAX = 4, // views of a split screen
                GOAL_MAX = 4  // goal icons of a view
            };

            CRadarIcon2DHockeyGoal(long nId);
            virtual ~CRadarIcon2DHockeyGoal();
            virtual void Main();
            virtual long SendEvent(long nEvent, void *pParam);
            virtual void Disp();

            void Initialize();
            void InitGoalIcons(ICON_PARAMETER *pParam);
            void DispGoalIcon(long nView, const bcs::FVEC3 &rCamAt, float fX, float fY);
            void CreateGoalIcon(ICON_PARAMETER *pParam, long nGoal, const char *pRootName, const char *pNullName,
                                const char *pBaseName, const char *pArrowName);
            void SetDispGoalIcon(long nGoal, int bDisp);
            // Not called; see the unit.
            void CheckGoalIcon();

        private:
            ICON_PARAMETER *m_pIconParam;           // 0x2C, VIEW_MAX * GOAL_MAX icons
            ICON_PARAMETER *m_pGoalIcon[VIEW_MAX];  // 0x30, each view's part of m_pIconParam
            long m_nGoalNum;                        // 0x40, goals of the court
    };
}

#endif /* !CRADARICON2DHOCKEYGOAL_HPP_ */
