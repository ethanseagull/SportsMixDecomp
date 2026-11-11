#include "game/msw/CC_CMN/SLIME/CcSquadFactory.hpp"
#include "game/msw/CChrCore.hpp"
#include "game/msw/CChrOptTask.hpp"
#include "game/msw/CGame.hpp"
#include "bcs/CMdlObj.hpp"
#include "bcs/CTaskMan.hpp"
#include "bcs/New.hpp"

// The original file is wider than this unit: the float constants and the
// node table are shared with the slime functions from 0x801C39D4 on.

namespace msw {
    namespace CC_CMN {
        namespace SLIME {

            namespace {
                // One squad model (names guessed).
                struct SQUAD_INFO {
                    const char *pMdlName;
                    const char *pSubName0;
                    const char *pSubName1;
                    const bcs::CAnmObjMotBase::NODE_EX_INFO *pNodeExInfo;
                    long nNode;
                    long nArg8;
                    long nArg9;
                    long nArg10;
                };

                // The node settings of the squad models' motions (contents
                // not mapped; the original shares it with other slime code).
                const unsigned short sNodeExInfo[12] = {0, 0, 0, 1, 3, 0, 2, 3, 0, 2, 3, 0};

                const SQUAD_INFO sSquadInfo[CcSquadFactory::SQUAD_NUM] = {
                    {"c16_SP_ITEM01", "c16_SP_ITEM01", "c16_SP_ITEM01",
                     reinterpret_cast<const bcs::CAnmObjMotBase::NODE_EX_INFO *>(sNodeExInfo), 0, 0x20, -1, -1},
                    {"c16_SP_ITEM02", "c16_SP_ITEM02", "c16_SP_ITEM02",
                     reinterpret_cast<const bcs::CAnmObjMotBase::NODE_EX_INFO *>(sNodeExInfo), 0, 0x20, -1, -1},
                    {"c16_SP_ITEM03", "c16_SP_ITEM03", "c16_SP_ITEM03",
                     reinterpret_cast<const bcs::CAnmObjMotBase::NODE_EX_INFO *>(sNodeExInfo), 0, 0x20, -1, -1},
                };
            }

            CcSquadFactory::CcSquadFactory(bcs::CTask *pParent, CChrCore *pChr, const float *pfSpeed)
                : bcs::CUnivTaskEx(pParent, -1)
            {
                for (long i = 0; i < SQUAD_NUM; i++) {
                    m_aTaskId[i] = 0;
                }
                Create_(pChr, pfSpeed);
            }

            CcSquadFactory::~CcSquadFactory()
            {
                for (long i = 0; i < SQUAD_NUM; i++) {
                    if (m_aTaskId[i] != 0) {
                        bcs::CTask *pTask = bcs::CTaskMan::GetTask(m_aTaskId[i]);
                        if (pTask != 0 && !pTask->IsDelete()) {
                            pTask->Delete();
                        }
                        m_aTaskId[i] = 0;
                    }
                }
            }

            void CcSquadFactory::Create_(CChrCore *pChr, const float *pfSpeed)
            {
                nw4r::g3d::ResFile res = pChr->GetChrOptResFile();
                if (!res.IsValid()) {
                    return;
                }
                nw4r::g3d::ResFile mot = pChr->GetChrOptMotResFile();

                for (long i = 0; i < SQUAD_NUM; i++) {
                    const SQUAD_INFO &rInfo = sSquadInfo[i];
                    CChrOptTask *pTask = new (0x100) CChrOptTask(CGame::GetTempParent(), 0);
                    if (pTask != 0) {
                        pTask->Create(pChr->GetMdl(), rInfo.nNode, res, mot, rInfo.pMdlName, rInfo.pSubName0,
                                      rInfo.pSubName1, rInfo.pNodeExInfo, rInfo.nArg8, rInfo.nArg9, rInfo.nArg10);
                        pTask->AnimPlay(1, 5.0f, pfSpeed[i]);
                        m_aTaskId[i] = pTask->GetId();
                    }
                }
                pChr->PlaySePos(0x486, 0, 0, -1, 0.0f, 1.0f, 0.0f);
            }

            void CcSquadFactory::AnimPlay(long, int, float) {}
        }
    }
}
