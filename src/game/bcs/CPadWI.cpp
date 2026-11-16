#include "bcs/CPadWI.hpp"
#include "bcs/CMainApp.hpp"
#include "revolution/kpad.h"

namespace bcs {

    // The buttons and stick of one remote as the engine keeps them each
    // frame. 0x30 bytes.
    struct PAD_WI_STATE {
        unsigned long m_nDown;    // 0x00
        unsigned long m_nUnk04;
        unsigned long m_nTrg;     // 0x08
        unsigned long m_nRelease; // 0x0C
        unsigned char m_Unk10[0x18 - 0x10];
        float m_fStickX;          // 0x18
        float m_fStickY;          // 0x1C
        unsigned char m_Unk20[0x30 - 0x20];
    };

    // One entry of a remote's rumble stack. 0x58 bytes.
    struct PAD_MOTOR_STATE {
        float m_aData[8][2];      // 0x00 (strength, time) pairs
        long m_nUnk40;
        long m_nUnk44;            // 0x44 negative: not paused by PushMotorAll
        long m_nPlay;             // 0x48 negative: stopped
        float m_fUnk4C;
        long m_nUnk50;
        long m_nUnk54;            // 0x54 non-zero: not resumed by PopMotorAll
    };

    // The rumble of one remote: a callback pair that switches the motor on
    // and off, and a stack of rumble states PushMotorAll / PopMotorAll move
    // through. 0x1CC bytes; its constructor (fn_800704E0) and Start are in
    // the part of the file not written yet.
    class CPadMotor {
        public:
            virtual ~CPadMotor();
            // (fn_80070584) Plays nNum (strength, time) pairs for nFrame frames.
            int Start(const float *pData, long nNum, long nFrame);

            void *m_pArg;                   // 0x04
            void (*m_pOn)(void *pArg);      // 0x08
            void (*m_pOff)(void *pArg);     // 0x0C
            PAD_MOTOR_STATE m_aStack[5];    // 0x10
            long m_nStack;                  // 0x1C8
    };
}

// The file's statics, defined in the part of the file before IsDown that is
// not written yet.
extern "C" {
    extern KPADStatus lbl_804CCE20[4][10];           // this frame's readings
    extern long lbl_804CF3A0[];                      // how many of them
    extern bcs::PAD_WI_STATE lbl_804CF3C0[4];
    extern bcs::CPadMotor lbl_804CF550[4];
    extern long lbl_804CFC80[4];                     // pointer (DPD) on
    extern long lbl_804CFC90[4];                     // saved by PushDPD
    extern long lbl_804CFCA0[];                      // Nunchuk style
    extern long lbl_805C09A0;                        // PushDPD depth

    // Restores the pointer settings saved by PushDPD once the last push is
    // popped; non-zero then.
    int fn_80070394();
}

namespace bcs {

    // A reading of remote nPad that came through without error.
    static inline int IsReadOK(long nIndex, long nPad)
    {
        if (lbl_804CF3A0[nPad] <= nIndex) {
            return 0;
        }
        long err = lbl_804CCE20[nPad][nIndex].wpad_err;
        if (err != 0 && err != -7) {
            return 0;
        }
        return 1;
    }

    // ... and that holds a pointer position.
    static inline int IsDPDOK(long nIndex, long nPad)
    {
        if (!IsReadOK(nIndex, nPad)) {
            return 0;
        }
        KPADStatus *pStatus = &lbl_804CCE20[nPad][nIndex];
        long fmt = pStatus->data_format;
        if (!((fmt >= 8 && fmt <= 9) || fmt == 2 || fmt == 5)) {
            return 0;
        }
        long dev = pStatus->dev_type;
        if (!((dev >= 0 && dev <= 2) || (dev >= 0xFB && dev <= 0xFC) || dev == 0xFF)) {
            return 0;
        }
        return pStatus->dpd_valid_fg >= 1;
    }

    static inline long GetReadIndex(long nPad)
    {
        for (long i = 0; i < lbl_804CF3A0[nPad]; i++) {
            if (IsReadOK(i, nPad)) {
                return i;
            }
        }
        return -1;
    }

    static inline long GetDPDIndex(long nPad)
    {
        for (long i = 0; i < lbl_804CF3A0[nPad]; i++) {
            if (IsDPDOK(i, nPad)) {
                return i;
            }
        }
        return -1;
    }

    static inline KPADStatus *GetStatus(long nIndex, long nPad)
    {
        if (nIndex < 0) {
            return 0;
        }
        return &lbl_804CCE20[nPad][nIndex];
    }

    int CPadWI::IsDown(unsigned long nButton, long nPad)
    {
        return (nButton & lbl_804CF3C0[nPad].m_nDown) != 0;
    }

    int CPadWI::IsTrg(unsigned long nButton, long nPad)
    {
        return (nButton & lbl_804CF3C0[nPad].m_nTrg) != 0;
    }

    int CPadWI::IsRelease(unsigned long nButton, long nPad)
    {
        return (nButton & lbl_804CF3C0[nPad].m_nRelease) != 0;
    }

    unsigned long CPadWI::GetDown(long nPad)
    {
        return lbl_804CF3C0[nPad].m_nDown;
    }

    unsigned long CPadWI::GetTrg(long nPad)
    {
        return lbl_804CF3C0[nPad].m_nTrg;
    }

    unsigned long CPadWI::GetRelease(long nPad)
    {
        return lbl_804CF3C0[nPad].m_nRelease;
    }

    float CPadWI::GetStickX(long nPad)
    {
        return lbl_804CF3C0[nPad].m_fStickX;
    }

    float CPadWI::GetStickY(long nPad)
    {
        return lbl_804CF3C0[nPad].m_fStickY;
    }

    int CPadWI::IsConnect(long nPad)
    {
        return GetStatus(GetReadIndex(nPad), nPad) != 0;
    }

    void CPadWI::EnableDPD(long nPad)
    {
        KPADEnableDPD(nPad);
        lbl_804CFC80[nPad] = 1;
    }

    void CPadWI::DisableDPD(long nPad)
    {
        KPADDisableDPD(nPad);
        lbl_804CFC80[nPad] = 0;
    }

    int CPadWI::IsEnableDPD(long nPad)
    {
        return lbl_804CFC80[nPad];
    }

    int CPadWI::PushDPD()
    {
        if (lbl_805C09A0 > 0) {
            lbl_805C09A0++;
            return 0;
        }
        for (long i = 0; i < 4; i++) {
            lbl_804CFC90[i] = lbl_804CFC80[i];
        }
        lbl_805C09A0++;
        return 1;
    }

    int CPadWI::PopDPD()
    {
        return fn_80070394();
    }

    int CPadWI::GetScreenPos_F(FVEC2 *pPos, long nPad)
    {
        KPADStatus *pStatus = GetStatus(GetDPDIndex(nPad), nPad);
        if (pStatus) {
            GXRenderModeObj &rmode = CMainApp::g_Rmode;
            Vec2 pos;
            KPADRect rect;
            rect.left = 0.0f;
            rect.top = 0.0f;
            rect.right = rmode.fbWidth;
            rect.bottom = rmode.efbHeight;
            KPADGetProjectionPos(&pos, &pStatus->pos, &rect,
                                 (float)(long)rmode.fbWidth / (float)(long)rmode.viWidth);
            pPos->x = 0.5f * (long)rmode.fbWidth + pos.x;
            pPos->y = 0.5f * (long)rmode.efbHeight + pos.y;
            return 1;
        }
        return 0;
    }

    int CPadWI::GetScreenPosDPD_F(FVEC2 *pPos, long nPad)
    {
        KPADStatus *pStatus = GetStatus(GetDPDIndex(nPad), nPad);
        if (pStatus) {
            pPos->x = pStatus->pos.x;
            pPos->y = pStatus->pos.y;
            return 1;
        }
        return 0;
    }

    int CPadWI::IsNunchukStyle(long nPad)
    {
        return lbl_804CFCA0[nPad];
    }

    int CPadWI::StartMotor(const float *pData, long nNum, long nFrame, long nPad)
    {
        return lbl_804CF550[nPad].Start(pData, nNum, nFrame);
    }

    int CPadWI::StartMotor(float fFrame, long nPad)
    {
        float data[2];
        data[0] = fFrame;
        data[1] = 0.0f;
        return lbl_804CF550[nPad].Start(data, 1, 1);
    }

    void CPadWI::StopMotor(long nPad)
    {
        CPadMotor &motor = lbl_804CF550[nPad];
        motor.m_aStack[motor.m_nStack].m_nPlay = -1;
        motor.m_pOff(motor.m_pArg);
    }

    void CPadWI::StopMotorAll()
    {
        long i;
        CPadMotor *pMotor;
        for (i = 0, pMotor = lbl_804CF550; i < 4; i++, pMotor++) {
            pMotor->m_aStack[pMotor->m_nStack].m_nPlay = -1;
            pMotor->m_pOff(pMotor->m_pArg);
        }
    }

    void CPadWI::PushMotorAll()
    {
        for (long i = 0; i < 4; i++) {
            CPadMotor &motor = lbl_804CF550[i];
            if (motor.m_nStack < 5) {
                PAD_MOTOR_STATE &state = motor.m_aStack[motor.m_nStack];
                if (state.m_nPlay >= 0 && state.m_nUnk44 >= 0) {
                    state.m_nPlay = -1;
                }
                motor.m_pOff(motor.m_pArg);
                motor.m_nStack++;
            }
        }
    }

    void CPadWI::PopMotorAll()
    {
        for (long i = 0; i < 4; i++) {
            CPadMotor &motor = lbl_804CF550[i];
            if (motor.m_nStack > 0) {
                motor.m_nStack--;
                PAD_MOTOR_STATE &state = motor.m_aStack[motor.m_nStack];
                if (state.m_nPlay >= 0 && state.m_nUnk54 == 0) {
                    motor.m_pOn(motor.m_pArg);
                }
            }
        }
    }
}
