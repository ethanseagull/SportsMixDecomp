#ifndef MSW_CSTAGE16BASE_HPP_
#define MSW_CSTAGE16BASE_HPP_

#include "bcs/BCS_Color.hpp"

namespace msw {

    class CStageMdlListNode;

    // (purpose guessed from the names) Stage 16, the star piece stage: a
    // second base class next to the sport's stage manager (DodgeF's
    // msw::CStageDodge16 has it at 0x244). A DOL class, size 0x1C: data
    // first, vptr at 0x18, like CStage06Base. Star pieces fall on the court
    // (PopStarPiece) and the sky models switch during a warp. The sport's
    // stage hands its hooks on (Stage16_*) and gives the tuning through the
    // GetParam_* functions, which it overrides. Field names and types are
    // guesses from the DOL's accesses.
    class CStage16Base {
        public:
            CStage16Base();

        private:
            float m_fPopTimer;  // 0x00, star piece pop timer (SetPopCnt, Stage16_Main_)
            long m_Unk04;       // 0x04
            float m_fUnk08;     // 0x08
            long m_Unk0C;       // 0x0C
            long m_nWarpState;  // 0x10, nonzero during a warp
            long m_Unk14;       // 0x14

        public:
            virtual ~CStage16Base();                                         // 0x08
            virtual void Stage16_Main_(float frame);                         // 0x0C
            virtual bool Stage16_IsGimmickReady() const = 0;                 // 0x10
            virtual void SetPopCnt();                                        // 0x14
            virtual void PopStarPiece();                                     // 0x18
            virtual void StarPiece_Clear();                                  // 0x1C
            virtual long StarPiece_GetNum() const;                           // 0x20
            virtual void Stage16_AddSwitchableMdl(CStageMdlListNode *pNode, long no); // 0x24
            virtual void Stage16_ResetSwitchableMdl();                       // 0x28
            virtual void Stage16_SwitchMdl(long no);                         // 0x2C
            virtual void Stage16_SwitchNextMdl();                            // 0x30
            virtual void DemoFog_Update();                                   // 0x34
            virtual void Warp_Reset();                                       // 0x38
            virtual void Warp_Cancel();                                      // 0x3C
            virtual float Warp_TimerConvert(float time) const;               // 0x40
            virtual void Warp_Update(float frame);                           // 0x44
            virtual void Warp_EffectUpdate(float frame);                     // 0x48
            virtual void Warp_Start();                                       // 0x4C
            virtual void Warp_OnEnd();                                       // 0x50
            // Emitted weak by the modules (DodgeF CStageDodge16).
            virtual bool Warp_IsNowWarp() const { return m_nWarpState != 0; } // 0x54
            virtual void Stage16_OnSceneEntry();                             // 0x58
            virtual void Stage16_OnPostInitialize();                         // 0x5C
            virtual void Stage16_OnEntranceEventStart();                     // 0x60
            virtual void Stage16_OnGameInit();                               // 0x64
            virtual void Stage16_OnGimmickReset();                           // 0x68
            virtual void Stage16_OnGimmickCancel();                          // 0x6C
            virtual void Stage16_OnGimmickStop();                            // 0x70
            virtual void Stage16_OnPeriodInit();                             // 0x74
            virtual void Stage16_OnTurnInit();                               // 0x78

            // Tuning, given by the sport's stage. Times are in frames.
            virtual float GetParam_StarPieceAreaZ() const = 0;           // 0x7C
            virtual float GetParam_StarPiecePopTime() const = 0;         // 0x80
            virtual float GetParam_StarPiecePopTimeRnd() const = 0;      // 0x84
            virtual float GetParam_StarPiecePopPosY() const = 0;         // 0x88
            virtual float GetParam_StarPieceSpeed() const = 0;           // 0x8C, per frame
            virtual float GetParam_StarPieceDropAngle() const = 0;       // 0x90, radians
            virtual long GetParam_StarPieceRate(long kind) const = 0;    // 0x94
            virtual long GetParam_StarPieceAddCoin(long kind) const = 0; // 0x98
            virtual long GetParam_StarPieceMaxNum() const = 0;           // 0x9C
            virtual long GetParam_StarPieceBoundNum() const = 0;         // 0xA0
            virtual float GetParam_StarPieceBoundReduceY() const = 0;    // 0xA4
            virtual float GetParam_StarPieceBoundReduceXZ() const = 0;   // 0xA8
            virtual bcs::BCS_Color GetParam_FlashColor() const = 0;      // 0xAC
            // Emitted weak by the modules (DodgeF CStageDodge16).
            virtual float GetParam_FlashTime() const { return 120.0f; }  // 0xB0
            virtual float GetParam_FlashGain() const = 0;                // 0xB4
            virtual float GetParam_FlashDecay() const = 0;               // 0xB8
            virtual float GetParam_WarpInterval() const = 0;             // 0xBC
            virtual float GetParam_WarpTime() const = 0;                 // 0xC0
            virtual float GetParam_WarpBlurShift() const = 0;            // 0xC4
            virtual float GetParam_WarpGameSpeed() const = 0;            // 0xC8
            virtual int GetParam_WarpAutoFeed() const;                   // 0xCC
    };
}

#endif /* !MSW_CSTAGE16BASE_HPP_ */
