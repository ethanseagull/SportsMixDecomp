#include "game/msw/CTournamentRouteMan_Stage31.hpp"

#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "nw4r/g3d/ResFile.hpp"
#include "game/msw/CRouteDataMove.hpp"
#include "game/msw/CRouteJumpPoints.hpp"
#include "game/msw/CTournamentMan.hpp"
#include "game/msw/EvDemo.hpp"

namespace msw {

    // A jump on one of the stage's mushrooms: a CRouteJumpPoints whose
    // character also follows the mushroom's own bounce (the node animation
    // "jump_kinoko_on_chara_set" of the mushroom model). 0xC0 bytes.
    class CRouteKinokoJump : public CRouteJumpPoints {
        public:
            CRouteKinokoJump(const _PARAM &param, float rate) : CRouteJumpPoints(param, rate)
            {
            }

            virtual ~CRouteKinokoJump();
            virtual CRouteBase *GetOffsetRoute(nw4r::g3d::ResFile file);
            virtual void *GetOffsetRes();
    };

    namespace {

        // msw::CLoadData entry of the map objects. (name guessed)
        const long LOADDATA_OBJ = 0x5C;

        // The two kinds of jump of this stage: between map points (routes
        // 4 to 6) and on a mushroom (routes 7 to 9).
        const CRouteJumpPoints::JUMP_DATA s_JumpDataPoints = {
            {1.0f / 30.0f, 1.0f / 30.0f, -20.0f, -20.0f, -20.0f, 5.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             0.0f},
            {0.0f, 0.0f, 0.0f},
            0.0f,
            1.0f,
            {-1, -1, -1, 0xF},
            0.0f,
            0,
            -1,
            -1,
            0xE29,
        };

        const CRouteJumpPoints::JUMP_DATA s_JumpDataKinoko = {
            {1.0f / 30.0f, 1.0f / 30.0f, -20.0f, -20.0f, 5.0f, 13.0f, 0.0f, 10.0f, 0.0f, 50.0f, 70.0f, 50.0f,
             50.0f, 50.0f},
            {50.0f, 0.0f, 0.0f},
            -3.0f,
            0.0f,
            {-1, 0x12, 0x14, 0x16},
            50.0f,
            0,
            -1,
            -1,
            0xE37,
        };

        // Start and end of the mushroom bounce route; both are the origin,
        // the route takes its path from the model.
        CRouteBase::_PARAM s_KinokoParam(bcs::FVEC3(0.0f, 0.0f, 0.0f), bcs::FVEC3(0.0f, 0.0f, 0.0f));

        // Where the sound of the shown map plays.
        bcs::FVEC3 s_DispSePos(-26.0f, 4.0f, -34.0f);
    }

    CRouteKinokoJump::~CRouteKinokoJump()
    {
    }

    CRouteBase *CRouteKinokoJump::GetOffsetRoute(nw4r::g3d::ResFile file)
    {
        CRouteDataMove *pRoute = new CRouteDataMove(s_KinokoParam, 1.0f, 6, 0);
        pRoute->SetNode(file, "jump_kinoko_on_chara_set", 1);
        return pRoute;
    }

    void *CRouteKinokoJump::GetOffsetRes()
    {
        long nIndex;
        return EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_kinoko", 0);
    }

    long CTournamentRouteMan_Stage31::GetChipType(long nPoint)
    {
        // The chip shown at each map point, -1 for none.
        static const signed char g_sPointToChipConvert[32] = {
            -1,   0x10, 0x10, 0x10, 0x11, 0x16, 0x15, 0x19, 0x11, 0x13, 0x14, 0x17, 0x11, 0x1A, 0x18, 0x12,
            -1,   0x01, 0x03, 0x05, 0x04, 0x0B, 0x0A, 0x0E, 0x02, 0x08, 0x09, 0x0C, 0x06, 0x0F, 0x0D, 0x07,
        };
        return g_sPointToChipConvert[nPoint];
    }

    void CTournamentRouteMan_Stage31::Initialize()
    {
        _CreateTimer(0x80);
        InitializeModel();
    }

    void CTournamentRouteMan_Stage31::InitializeModel()
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        pMan->SetupObjModelEx(0, 0, "map_cup", 0, bcs::FVEC3(1.0f, 0.0f, -32.0f), 1, 2, 1, 0, -1);
        SetCrystalColor(0);
        pMan->SetupObjModel(2, 0, "map_dokan", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModel(3, 0, "map_dokan", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        pMan->SetupObjModel(4, 1, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 1, 1, 1, 0, -1);
        pMan->SetupObjModel(5, 2, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 1, 1, 1, 0, -1);
        for (long i = 6; i <= 17; i++) {
            pMan->SetupObjModel(i, 0, "map_chips", 0, bcs::FVEC3(0.0f, 0.0f, 0.0f), 1, 1, 0, 0, -1);
        }
        for (long i = 18; i <= 29; i++) {
            pMan->SetupObjModel(i, 0, "map_kinoko", 1, bcs::FVEC3(0.0f, 0.0f, 0.0f), 0, 2, 0, 0, -1);
        }
    }

    int CTournamentRouteMan_Stage31::GetParamData(long nId, GET_PARAM &param)
    {
        if (nId == 0) {
            param.fValue = m_fParam0;
            return 1;
        }
        return 0;
    }

    CRouteBase *CTournamentRouteMan_Stage31::CreateRoute(long nType, long nFrom, long nTo)
    {
        CRouteBase::_PARAM param;
        CRouteBase::_SetParam(param, nFrom, nTo, 0);

        switch (nType) {
            case 4:
            case 5:
            case 6: {
                CRouteJumpPoints *pRoute = new CRouteJumpPoints(param, 1.0f);
                pRoute->SetJumpData(&s_JumpDataPoints);
                return pRoute;
            }
            case 7:
            case 8:
            case 9: {
                CRouteKinokoJump *pRoute = new CRouteKinokoJump(param, 1.0f);
                pRoute->SetJumpData(&s_JumpDataKinoko);
                return pRoute;
            }
            case 10: {
                long nIndex;
                nw4r::g3d::ResFile file(
                    static_cast<nw4r::g3d::ResFileData *>(EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_roots", 0)));
                param.fRotY = 1.5707964f;
                CRouteDataMove *pRoute = new CRouteDataMove(param, 1.0f, 40, 0);
                pRoute->SetNode(file, "root_path01", 0L);
                return pRoute;
            }
            case 11: {
                long nIndex;
                nw4r::g3d::ResFile file(
                    static_cast<nw4r::g3d::ResFileData *>(EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_roots", 0)));
                param.fRotY = 1.5707964f;
                CRouteDataMove *pRoute = new CRouteDataMove(param, 1.0f, 40, 0);
                pRoute->SetNode(file, "root_path02", 0L);
                return pRoute;
            }
            case 12: {
                long nIndex;
                nw4r::g3d::ResFile file(
                    static_cast<nw4r::g3d::ResFileData *>(EvDemo::GetArcResDataMdl(nIndex, LOADDATA_OBJ, "map_roots", 0)));
                param.fRotY = 1.5707964f;
                CRouteDataMove *pRoute = new CRouteDataMove(param, 1.0f, 40, 0);
                pRoute->SetNode(file, "root_path03", 0L);
                return pRoute;
            }
        }
        return _CreateRoute(param, nType, nFrom, nTo);
    }

    void CTournamentRouteMan_Stage31::TimerEvent(long nEvent, long nParam, float fParam, unsigned long ulParam, int b)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();

        switch (nEvent) {
            case 0xC: { // a rope bridge starts to sway
                const char *pAnim;
                switch (nParam) {
                    case 4:
                        pAnim = "s31_turi_hashi_1_ground";
                        break;
                    case 5:
                        pAnim = "s31_turi_hashi2_ground";
                        break;
                    default:
                        pAnim = 0;
                        break;
                }
                _StageAnim(pAnim, nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                _AddTimer(0x18, m_nTimerGroup, 44.0f + m_fTimer, nParam, fParam, ulParam);
                break;
            }
            case 0xD: // ... and stops
                switch (nParam) {
                    case 4:
                        _StageAnimEnd("s31_turi_hashi_1_ground", nParam, fParam, ulParam, 0x1F, 1, -1);
                        break;
                    case 5:
                        _StageAnimEnd("s31_turi_hashi2_ground", nParam, fParam, ulParam, 0x1F, 1, -1);
                        break;
                }
                break;
            case 0x18:
                _StageSe(0xE32, nParam, fParam, ulParam, -1);
                break;
            case 0xE:
                _StageAnim("u_01", nParam, fParam, ulParam, 0, 0, 0.0f, 3, 1);
                _StageEff(0x568, nParam, fParam, ulParam, 3, 0, 0);
                _StageEffAttach(0x56B, "obj_04", nParam, fParam, ulParam, 3, 0, 0);
                _StageEffFadeIn(0x56C, 10.0f, nParam, fParam, ulParam, 0, 0, 0);
                _StageSe(0xE33, nParam, fParam, ulParam, -1);
                break;
            case 0xF:
                _StageAnim("u_02", nParam, fParam, ulParam, 0x10, 0, 0.0f, 1, 1);
                _StageEff(0x569, nParam, fParam, ulParam, 3, 0, 0);
                _StageSe(0xE34, nParam, fParam, ulParam, 0x11B);
                break;
            case 0x10:
                _StageAnim("u_03", nParam, fParam, ulParam, 0, 0, 0.0f, 3, 1);
                _StageEff(0x56A, nParam, fParam, ulParam, 3, 0, 0);
                _StageEff(0x56D, nParam, fParam, ulParam, 3, 0, 0);
                _AddTimer(0x11, m_nTimerGroup, 100.0f + m_fTimer, nParam, fParam, ulParam);
                pMan->StopSe(0.0f, 0x11B, 0x11C);
                _StageSe(0xE35, nParam, fParam, ulParam, -1);
                break;
            case 0x11:
                _StageEffFadeOut(10.0f, nParam, fParam, ulParam);
                break;
            case 0x12: // mushroom jump: take-off
                _StageEff(0x54F, nParam, fParam, ulParam, 5, 0, 0);
                _StageEff(0x550, nParam, fParam, ulParam, 3, 0, 0);
                _AddTimer(0x13, m_nTimerGroup, 2.0f + m_fTimer, nParam, fParam, ulParam);
                _AddTimer(0x3FA, m_nTimerGroup, 2.0f + m_fTimer, nParam, 0.1f, ulParam);
                break;
            case 0x13:
                _StageAnim("jump_kinoko_st", nParam, fParam, ulParam, 0, 0, 0.0f, 1, 1);
                _StageSe(0xE36, nParam, fParam, ulParam, -1);
                break;
            case 0x14: // mushroom jump: landing
                _StageAnim("jump_kinoko_end", nParam, fParam, ulParam, 0x3F7, 0, 0.0f, 1, 1);
                _AddTimer(0x15, m_nTimerGroup, 20.0f + m_fTimer, nParam, fParam, ulParam);
                break;
            case 0x15:
                _AddTimer(0x3F9, m_nTimerGroup, m_fTimer, nParam, 0.1f, ulParam);
                _StageEff(0x551, nParam, fParam, ulParam, 3, 0, 0);
                break;
            case 0x16: // a character lands on a mushroom
                _StageAnim("jump_kinoko_on", nParam, 0.0f, ulParam, 0, 0, fParam + m_fTimerOver, 1, 1);
                break;
            case 0x17:
                break;
            case 0x3F6:
                m_nUnk584 = 0;
                break;
            case 0xB: // all chips flash
                if (!pMan->IsPlaySe(0x11A)) {
                    _StageSe(0xE1C, nParam, fParam, ulParam, 0x11A);
                }
                for (long i = 0; i < 128; i++) {
                    if (m_apChip[i] != 0) {
                        _StageChipFade(i, 1.0f, 0.0f);
                        _StageChipFade(i, 0.0f, 1.0f / fParam);
                        _StageChipEff(0x567, i, 1.0f / fParam, ulParam, 3, 0, 0);
                    }
                }
                _AddTimer(0, m_nTimerGroup, m_fTimer + fParam, nParam, fParam, ulParam);
                break;
            default:
                _DefTimerEvent(nEvent, nParam, fParam, ulParam, b);
                break;
        }
    }

    void CTournamentRouteMan_Stage31::PushCamera(CRouteBase::_PUSH_INFO *pInfo, long n, float f, CEventModel *pModel)
    {
        _DefPushCamera(pInfo, n, f, pModel);
    }

    void CTournamentRouteMan_Stage31::StageDispSe(int bDisp)
    {
        CTournamentMan *pMan = CTournamentMan::GetInstance();
        if (bDisp) {
            pMan->PlaySe(0xE39, s_DispSePos, pMan->GetSeSlot(0x120, 0x130), 0.0f, 1.0f, 0.0f, 1);
        } else {
            pMan->StopSe(0.0f, 0x120, 0x130);
        }
    }

    CTournamentRouteMan_Stage31::~CTournamentRouteMan_Stage31()
    {
    }
}
