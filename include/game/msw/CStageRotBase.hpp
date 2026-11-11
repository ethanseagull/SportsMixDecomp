#ifndef MSW_CSTAGEROTBASE_HPP_
#define MSW_CSTAGEROTBASE_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CStageMan.hpp"

namespace msw {

    class CStageMdl;
    class CStageMdlListNode;

    // (purpose guessed from the names) A stage that rolls: the court sits on
    // a ship that tilts about Z and drifts sideways, and the characters slide
    // with the tilt (GetFloorFlow__). A DOL class (constructor
    // __ct__Q23msw13CStageRotBaseFv), the first base of msw::CStage15Base:
    // data first, vptr at 0x3C. The sport's stage hands its hooks on and
    // gives the tuning through the GetParam_* functions.
    //
    // The order of the virtual functions follows __vt__Q23msw13CStageDodge15
    // (DodgeF). The modules emit weak copies of the functions with bodies
    // here; the GetParam_* functions a module class overrides have no DOL
    // default with a name, so they are declared pure. Field names and the
    // return types not visible in a body are guesses.
    class CStageRotBase {
        public:
            CStageRotBase();

        private:
            float m_Unk00;                       // 0x00
            float m_Unk04;                       // 0x04
            float m_fRotTheta;                   // 0x08
            float m_fRotRate;                    // 0x0C
            float m_Unk10;                       // 0x10
            CStageMdl *m_pScrollNull;            // 0x14
            CStageMdl *m_pScrollNull2;           // 0x18
            float m_fWorldRotZ;                  // 0x1C
            float m_Unk20;                       // 0x20
            CStageMdlListNode *m_apRollMdl[4];   // 0x24
            long m_nRollMdlNum;                  // 0x34
            unsigned long m_Unk38;               // 0x38

        public:
            virtual ~CStageRotBase();                                     // 0x08
            virtual const CStageMan *GetStageMan() const = 0;             // 0x0C
            virtual void StageRot_Main_(float frame);                     // 0x10
            virtual void SetScrollNull(CStageMdl *pMdl) { m_pScrollNull = pMdl; }   // 0x14
            virtual void SetScrollNull2(CStageMdl *pMdl) { m_pScrollNull2 = pMdl; } // 0x18
            // Adds one of the (up to four) models that roll with the ship.
            virtual bool AddRollMdlNode(CStageMdlListNode *pNode)          // 0x1C
            {
                if (m_nRollMdlNum < 4) {
                    m_apRollMdl[m_nRollMdlNum++] = pNode;
                } else {
                    return false;
                }
                return true;
            }
            virtual bool IsRotReady_() const;                             // 0x20
            virtual void InitWorldRot__();                                // 0x24
            virtual float GetWorldRotZ__() const { return m_fWorldRotZ; } // 0x28
            virtual bool IsBalanced() const;                              // 0x2C
            virtual void StartCameraQuake();                              // 0x30
            virtual long GetFloorAttribute__(const bcs::FVEC3 &rPos) const { return 2; } // 0x34
            virtual void GetFloorFlow__(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                        CStageMan::FLOW_TYPE type) const; // 0x38
            virtual float GetRotTheta__() const { return m_fRotTheta; }   // 0x3C
            virtual float GetRotRate__() const { return m_fRotRate; }     // 0x40
            virtual void StageRot_OnSceneEntry();                         // 0x44
            virtual void StageRot_OnGimmickReset();                       // 0x48
            virtual void StageRot_OnGimmickCancel();                      // 0x4C

            // Tuning, given by the sport's stage. Times are in frames.
            virtual float GetParam_ShipCycle() const = 0;                 // 0x50
            virtual float GetParam_ShipStopTime() const = 0;              // 0x54
            virtual float GetParam_ShipStopTimeRnd() const = 0;           // 0x58
            virtual float GetParam_ShipStopTime2() const { return -1.0f; }    // 0x5C
            virtual float GetParam_ShipStopTimeRnd2() const { return -1.0f; } // 0x60
            virtual float GetParam_ChrFlowVel() const = 0;                // 0x64
            virtual float GetParam_ShipTransWidth() const { return 3.0f; } // 0x68
            virtual float GetParam_ShipRotWidth() const = 0;              // 0x6C, radians
            virtual float GetParam_ShipRotInterval() const { return 0.0f; } // 0x70
            virtual bool GetParam_IsShipRotEnable() const { return true; } // 0x74
            virtual long GetParam_ShipSe() const = 0;                     // 0x78
            virtual float GetParam_BgRotSpeed() const { return 0.0f; }    // 0x7C
    };
}

#endif /* !MSW_CSTAGEROTBASE_HPP_ */
