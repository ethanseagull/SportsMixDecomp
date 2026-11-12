#include "bcs/CWifiPad.hpp"
#include "bcs/CPadWI.hpp"
#include "bcs/CAccInputMan.hpp"
#include "bcs/CCameraMan.hpp"
#include "bcs/CCamera.hpp"

namespace bcs {

    int CWifiPad::IsNunchukStyle(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->IsNunchukStyle(nPad);
        }
        return CPadWI::IsNunchukStyle(nPad);
    }

    int CWifiPad::IsDown(unsigned long nButton, long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->IsDown(nButton, nPad);
        }
        return CPadWI::IsDown(nButton, nPad);
    }

    int CWifiPad::IsTrg(unsigned long nButton, long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->IsTrg(nButton, nPad);
        }
        return CPadWI::IsTrg(nButton, nPad);
    }

    int CWifiPad::IsRelease(unsigned long nButton, long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->IsRelease(nButton, nPad);
        }
        return CPadWI::IsRelease(nButton, nPad);
    }

    unsigned long CWifiPad::GetDown(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetDown(nPad);
        }
        return CPadWI::GetDown(nPad);
    }

    unsigned long CWifiPad::GetTrg(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetTrg(nPad);
        }
        return CPadWI::GetTrg(nPad);
    }

    unsigned long CWifiPad::GetRelease(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetRelease(nPad);
        }
        return CPadWI::GetRelease(nPad);
    }

    float CWifiPad::GetStickX(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStickX(nPad);
        }
        return CPadWI::GetStickX(nPad);
    }

    float CWifiPad::GetStickY(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStickY(nPad);
        }
        return CPadWI::GetStickY(nPad);
    }

    float CWifiPad::GetCameraAngle(long nView, long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetCameraAngle(nPad);
        }

        CCamera *pCamera = CCameraMan::GetActiveCamera(nView);
        if (pCamera != 0) {
            float fAngle = pCamera->GetViewData().m_Unk4C;
            while (fAngle > 6.2831855f) {
                fAngle -= 6.2831855f;
            }
            while (fAngle < 0.0f) {
                fAngle += 6.2831855f;
            }
            return fAngle;
        }
        return 0.0f;
    }

    void CWifiPad::StopMotor(long nPad)
    {
        if (IsInMatch()) {
            nPad = ToLocalPadID(nPad);
        }
        if (nPad >= 0) {
            CPadWI::StopMotor(nPad);
        }
    }

    long CWifiPad::GetStrokeRaw0(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStrokeRaw0(nPad);
        }
        return CAccInputMan::GetStrokeRaw0(nPad);
    }

    long CWifiPad::GetStrokeRaw1(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStrokeRaw1(nPad);
        }
        return CAccInputMan::GetStrokeRaw1(nPad);
    }

    long CWifiPad::GetStrokeRaw2(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStrokeRaw2(nPad);
        }
        return CAccInputMan::GetStrokeRaw2(nPad);
    }

    long CWifiPad::GetStrokeRaw3(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStrokeRaw3(nPad);
        }
        return CAccInputMan::GetStrokeRaw3(nPad);
    }

    long CWifiPad::GetStrokeRaw4(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStrokeRaw4(nPad);
        }
        return CAccInputMan::GetStrokeRaw4(nPad);
    }

    long CWifiPad::GetStroke0(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStroke0(nPad);
        }
        return CAccInputMan::GetStroke0(nPad);
    }

    long CWifiPad::GetStroke1(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStroke1(nPad);
        }
        return CAccInputMan::GetStroke1(nPad);
    }

    long CWifiPad::GetStroke2(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStroke2(nPad);
        }
        return CAccInputMan::GetStroke2(nPad);
    }

    long CWifiPad::GetStroke3(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStroke3(nPad);
        }
        return CAccInputMan::GetStroke3(nPad);
    }

    long CWifiPad::GetStroke4(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetStroke4(nPad);
        }
        return CAccInputMan::GetStroke4(nPad);
    }

    int CWifiPad::IsTateStroke(long nPad)
    {
        if (GetStroke0(nPad) == 4) {
            return 1;
        }
        return 0;
    }

    int CWifiPad::IsYokoStroke(long nPad)
    {
        if (GetStroke0(nPad) == 5) {
            return 1;
        }
        return 0;
    }

    void CWifiPad::ResetAcc(float fParam, long nPad)
    {
        if (IsInMatch()) {
            dwc::CDwc::GetInstance()->ResetAcc(nPad);
            nPad = ToLocalPadID(nPad);
        }
        if (nPad >= 0) {
            CAccInputMan::ResetAcc(nPad, fParam);
        }
    }

    float CWifiPad::GetScratchPower(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetScratchPower(nPad);
        }
        return CAccInputMan::GetScratchPower(nPad);
    }

    float CWifiPad::GetNunchukPower(long nPad)
    {
        if (IsInMatch()) {
            return dwc::CDwc::GetInstance()->GetNunchukPower(nPad);
        }
        return CAccInputMan::GetNunchukPower(nPad);
    }

    int CWifiPad::IsBothHandStroke(float fNunchuk, float fRemocon, long nPad)
    {
        float fNunchukPower = GetNunchukPower(nPad);
        float fRemoconPower = GetScratchPower(nPad);
        if (fNunchuk <= 0.0f) {
            fNunchuk = 0.2f;
        }
        if (fRemocon <= 0.0f) {
            fRemocon = 0.2f;
        }
        if (fRemoconPower >= fRemocon && fNunchukPower >= fNunchuk) {
            return 1;
        }
        return 0;
    }

    int CWifiPad::IsNunchukStroke(float fPower, long nPad)
    {
        float fNunchukPower = GetNunchukPower(nPad);
        if (fPower <= 0.0f) {
            fPower = 0.2f;
        }
        if (fNunchukPower >= fPower) {
            return 1;
        }
        return 0;
    }

    int CWifiPad::IsRemoconStroke(float fPower, long nPad)
    {
        float fRemoconPower = GetScratchPower(nPad);
        if (fPower <= 0.0f) {
            fPower = 0.2f;
        }
        if (fRemoconPower >= fPower) {
            return 1;
        }
        return 0;
    }
}
