#ifndef CTOURNAMENTMAN_HPP_
#define CTOURNAMENTMAN_HPP_

#include "bcs/BCS_Color.hpp"
#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/TournamentWork.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CCameraAnm;
    class CMdlObj;
    class CSndTask;
}

namespace msw {

    class CEventCameraNull;
    class CEventModel;
    class CEffectCluster;
    class CEvNodeData;
    class CRouteBase;
    class CRouteMove;
    class CStageMan;
    class CTournamentCamera;

    namespace vm {
        class CVMData;
    }

    // The tournament map's manager (module TourF, game/tour/
    // CTournamentMan.cpp): the map's characters, objects, effects, sounds,
    // cameras and the route graph. A task, 0x41E4 bytes (CreateInstance
    // allocates that much); the layout is read from the functions of that
    // file. Member names, and parameter names where the symbols give none,
    // are guessed.
    class CTournamentMan : public bcs::CTask {
            // (added for TourF game/tour/CTournamentStep.cpp, which reads
            // m_pInstance, m_ulMoveStop and the route arrows directly)
            friend class CTournamentStep;

        public:
            // How many of each kind of slot the manager keeps.
            enum {
                CHR_MAX = 20,
                SE_MAX = 305,
                EFFECT_MAX = 494,
                OBJ_MAX = 207,
                POINT_MAX = 128,
                LINK_MAX = 0x200,
                MOVE_MAX = 0x200
            };

            static CTournamentMan *CreateInstance(bcs::CTask *pParent, long nView);
            static CTournamentMan *GetInstance();
            static CTournamentMan *GetPtr();

            virtual ~CTournamentMan();
            virtual void Main();
            virtual void Disp();

            // Called once the map's files are loaded: makes the effect,
            // layout and cockpit tasks.
            void SetupDataLoadEnd(int);
            void UpdateFrame(float fFrame);

            // The rule scripts (msw::vm::CVMData) of the map.
            void ScriptLoad(long nFile, long nScript);
            void ScriptBindData(long nFile, long nScript);
            vm::CVMData *GetScript(long nScript);

            // Map characters: the first member of every team on the map
            // (character slot = team), the other members of team nTeam, and
            // one character. bDisp shows it at once.
            void SetupChrModelLeader(int bDisp);
            void SetupChrModelSub(long nTeam, long nGroup, int bDisp);
            CEventModel *SetupChrModel(long nIndex, long nChr, long nColor, int bStar, long nTeam, long nNo,
                                       int bDisp);
            // Deletes the two objects the manager keeps for character
            // `nIndex`; with bDestroy set it also has msw::CEvDemoRes
            // destroy the character's model. (parameter names guessed)
            void CleanChrModel(long nIndex, int bDestroy);
            // The same for object `nIndex`. (parameter names guessed)
            void CleanObjModel(long nIndex, int bDestroy);

            // Creates map object `nIndex` from model `pName` (variant nSub)
            // unless it exists already, puts it at vPos and returns it.
            // The other parameters go to msw::CEvDemoRes's display setup;
            // pAnim (may be 0) is played on it and nPriority (-1 for none)
            // is its draw priority. Ex is the variant the stage's main model
            // (the cup) uses. (names guessed)
            CEventModel *SetupObjModel(long nIndex, long nSub, char *pName, long nDisp, const bcs::FVEC3 &vPos,
                                       long nLayer, long nGroup, int b, const char *pAnim, long nPriority);
            CEventModel *SetupObjModelEx(long nIndex, long nSub, char *pName, long nDisp, const bcs::FVEC3 &vPos,
                                         long nLayer, long nGroup, int b, const char *pAnim, long nPriority);
            // Makes pModel map object nIndex, deleting the one there.
            void SetupObj(long nIndex, CEventModel *pModel);

            // The stage model of the map (with its route manager) or of an
            // event demo.
            void SetupStageDemoModel(long nStage);
            void SetupStageModel(long nStage);

            // The scene file with the map's camera and node animations.
            void SetSceneData(nw4r::g3d::ResFile file);
            void SetSceneOffset(const bcs::FVEC3 &vOffset, const bcs::FVEC3 &vRot);
            // Makes pModel follow node pNode of the node animation set last
            // (or of scene camera nCamera's animation, number nNo).
            CEventModel *SetSceneNode(CEventModel *pModel, const char *pNode, int bDisp, int bMove);
            CEventModel *SetSceneNode(long nNo, long nCamera, short nStart, CEventModel *pModel, const char *pNode,
                                      int bDisp, int bMove);

            // Shows or hides the characters whose bit is set in ulMask.
            void SetChrDisp(int bDisp, unsigned long ulMask);
            // The first and last move frame of the characters in ulMask.
            void GetChrMoveFrame(float &fMin, float &fMax, unsigned long ulMask);
            int IsMoveEnd(unsigned long ulMask);

            // Character moves along routes and lines.
            unsigned long SetMoveRoute(long nIndex, int bFootSe, unsigned short *pRoute, long nNum, float fMin,
                                       int bLast, float fFrame);
            unsigned long SetMoveLine(long nIndex, long nPoint, float fLen, float fFrame, float fRate,
                                      float fMul);
            unsigned long CleanMoveRoute(long nIndex, int bReset);
            void InitMoveRoute(long nTeam);
            void InitMoveRouteRot(long nTeam);
            // Records the routes pRoute[0..nNum) as the last routes of team
            // nTeam (TournamentWork::TEAM_INFO::aRoute).
            void PushMoveRoute(long nTeam, unsigned short *pRoute, long nNum);
            // Sets up the route choice at map point nPoint: the routes of
            // pDir (nNum of them) and the arrows over character nChr.
            void SetupSelect(CRouteBase **ppRoute, unsigned char nPoint, const unsigned char *pDir, long nNum,
                             long nRoute, long nChr, float fHeight, float f1, float f2);

            // The route graph (see TournamentWork::MakePointData).
            void MakePointData(long nLink);
            // Returns the number of routes written to pRoute.
            long MakeRouteSelect(unsigned short *pRoute, unsigned char nStart, unsigned char nGoal,
                                 unsigned char nMask, const unsigned char *pVia, float fRand);

            // Map effects, kept in the slots nFirst..nLast: a free slot, and
            // effect nEff started in slot nSlot (0 when it did not start).
            // (names and return types guessed)
            long GetEffectSlot(long nFirst, long nLast);
            CEffectCluster *GetEffect(long nSlot);
            CEffectCluster *SetEffectPoint(long nSlot, long nEff, long nPoint, long n, unsigned long ul1,
                                           unsigned long ul2);
            CEffectCluster *SetEffect(long nSlot, long nEff, long n, unsigned long ul1, unsigned long ul2);
            CEffectCluster *AttachEffectModel(long nSlot, CEventModel *pModel, long nEff, long n,
                                              unsigned long ulNode, const bcs::FVEC3 *pPos, unsigned long ulFlag,
                                              unsigned long ul1, unsigned long ul2);
            CEffectCluster *AttachEffectCamera(long nSlot, long nEff, const bcs::FVEC3 *pPos, long n,
                                               unsigned long ulFlag, unsigned long ul1, unsigned long ul2);
            CEffectCluster *AttachEffectAuto(long nSlot, bcs::CMdlObj *pMdl, long nEff, long n);
            void StopEffect(long nFirst, long nLast);
            void EraseEffect(long nFirst, long nLast);

            // Map sounds, kept in the slots nFirst..nLast. (names and
            // return types guessed)
            long GetSeSlot(long nFirst, long nLast);
            bcs::CTask *PlaySe(long nSe, const bcs::FVEC3 *pPos, long nSlot, float fParam1, float fVolume,
                               float fParam3, unsigned long ulFlag);
            bcs::CTask *PlaySe(long nSe, const bcs::FVEC3 &vPos, long nSlot, float fParam1, float fVolume,
                               float fParam3, unsigned long ulFlag);
            void StopSe(float fFade, long nFirst, long nLast);
            int IsPlaySe(long nSlot);

            // The scene cameras (of the file given to SetSceneData) of
            // camera group nGroup.
            short GetSceneCameraIndex(long nGroup, short nStart);
            short GetSceneCameraIndex(long nGroup, const char *pName);

            // The map camera.
            void SetCameraTarget(const bcs::FVEC3 *pTarget);
            float SetCameraRate(float fAdd, float fMul, float fMin);
            float SetCameraRateFrame(float fFrame, float fAdd, float fMul, float fMin);
            void SetCameraDirect(const bcs::FVEC3 &vPos, long nCamera);
            void SetCameraPoint(long nPoint, long nCamera);
            // Where the tournament camera is now.
            void GetCameraPos(bcs::FVEC3 &pos);
            void LeapDataSceneCamera(long nGroup, short nStart, unsigned long ulFlag);
            int SetDataSceneCamera(long nGroup, short nStart, float fFrame, unsigned long ulFlag);
            int SetCameraActive(short nMode);
            int IsCameraEnd();

            // The markers over the player characters and their shadows.
            void CreatePlayerMarker(const char *pName, long nChr, long nType, int bDisp, long nIndex);
            void PlayerMarkerSetPosY(float fY, long nIndex);
            void PlayerMarkerDisp(int bDisp, long nIndex, int bInvert);
            void PlayerMarkerFead(float fAlpha, float fSpeed, long nIndex);
            void PlayerShadowDisp(int bDisp, long nIndex, int bInvert);
            void PlayerShadowFead(float fAlpha, float fSpeed, long nIndex);

            // (name guessed) Map object nIndex (see SetupObjModel), 0 when
            // there is none.
            CEventModel *GetObjModel(long nIndex) const { return m_apObjModel[nIndex]; }
            // (name guessed) Map character nIndex, 0 when there is none.
            CEventModel *GetChrModel(long nIndex) const { return m_apChrModel[nIndex]; }
            // The same under the name the original gives it (TourF
            // game/tour/CTournamentStep.cpp has a weak copy, GetChr__...CFl).
            CEventModel *GetChr(long nIndex) const { return m_apChrModel[nIndex]; }

        private:
            CTournamentMan(bcs::CTask *pParent);

            void _Init(long nView);
            void _Destory();

            // The effect in slot nSlot, 0 when there is none or it ended.
            unsigned long GetEffectHandle(long nSlot) const
            {
                if (static_cast<unsigned long>(nSlot) <= EFFECT_MAX - 1) {
                    return m_aEffect[nSlot];
                }
                return 0;
            }
            void SetEffectHandle(long nSlot, unsigned long ulHandle)
            {
                if (static_cast<unsigned long>(nSlot) <= EFFECT_MAX - 1) {
                    m_aEffect[nSlot] = ulHandle;
                }
            }
            // The sound in slot nSlot, 0 when there is none or it ended.
            bcs::CSndTask *GetSe(long nSlot) const;

            // CTask ends with a short at 0x30; without this the compiler puts
            // the byte-aligned colours in its tail padding at 0x32.
            unsigned char m_Unk32[2];          // 0x32
            bcs::BCS_Color m_Color;            // 0x34: the map colour faded to
            bcs::BCS_Color m_ColorStart;       // 0x38: the map colour faded from
            float m_fColorRate;                // 0x3C
            float m_fColorSpeed;               // 0x40
            CStageMan *m_pStageMan;            // 0x44
            CEvNodeData *m_pNodeData;          // 0x48
            bcs::CCameraAnm *m_pCameraAnm;     // 0x4C
            CTournamentCamera *m_pCamera;      // 0x50
            CEventCameraNull *m_pEventCamera;  // 0x54
            float m_fFovy;                     // 0x58: the view's field of view before the map
            short m_nCameraMode;               // 0x5C: -1 none, 0 map camera, 1 scene camera then map, 3 scene camera
            short m_nSceneCamera;              // 0x5E: the scene camera after the one playing, -1 none
            unsigned long m_ulMoveStop;        // 0x60: characters whose move does not advance
            float m_afMarkerPhase[2];          // 0x64: the pulse of the two player markers
            bcs::FVEC3 m_vSceneOffset;         // 0x6C
            bcs::FVEC3 m_vSceneRot;            // 0x78
            nw4r::g3d::ResFile m_SceneFile;    // 0x84
            vm::CVMData *m_pScript;            // 0x88
            CEventModel *m_apChrModel[CHR_MAX]; // 0x8C
            CRouteMove *m_apChrMove[CHR_MAX];  // 0xDC
            unsigned long m_aSe[SE_MAX];       // 0x12C: sound task handles
            unsigned long m_aEffect[EFFECT_MAX]; // 0x5F0: effect handles
            CEventModel *m_apObjModel[OBJ_MAX]; // 0xDA8; 195 to 198 are the route arrows, 200 and 201 the player markers
            short m_aPointHead[POINT_MAX];     // 0x10E4
            TournamentWork::_LINK_POINT m_aLink[LINK_MAX]; // 0x11E4
            TournamentWork::_MOVE_POINT m_aMove[MOVE_MAX]; // 0x21E4

            static CTournamentMan *m_pInstance;
    }; // size 0x41E4
}

#endif /* !CTOURNAMENTMAN_HPP_ */
