#ifndef CGAMECAMERA_HPP_
#define CGAMECAMERA_HPP_

#include "bcs/CCamera.hpp"

namespace msw {

    // Size 0x254 (from its allocation site). The camera of a match; each
    // sport derives its own. The class lives in the DOL: its constructor,
    // destructor and most virtuals are there, the four inline virtuals are
    // emitted weak by the module units that emit a derived vtable (BasketF
    // game/basket/CCameraBasket.cpp). Vtable from the DOL (17 words).
    // Return types of the virtuals are guessed unless stated. The inline
    // virtuals are declared in the reverse of the order the module emits
    // them (MessageHandler, GetFrameRateF, IsNormalCamMode, GetCamKind);
    // overrides keep their base's vtable slot wherever they are declared.
    class CGameCamera : public bcs::CCamera {
        public:
            // (contents not mapped)
            enum MSG_ID {};
            struct MSG_PARAM;

            CGameCamera(long view, short priority);
            virtual ~CGameCamera();
            virtual void Disp();
            virtual long GetCamKind() const { return 3; }
            virtual void MainCamera();
            virtual int OnActive();
            virtual void Reset();
            virtual void StartArtsEventDemo();
            virtual void EndArtsEventDemo();
            virtual bool IsNormalCamMode() const { return true; }
            virtual bool CanbeMain() const;
            virtual bool IsOffense() const;
            virtual float GetFrameRateF() const
            {
                if (m_UnkFC & 0x80) {
                    return 0.0f;
                }
                return bcs::CMainApp::GetAppFrameRateF();
            }
            virtual int MessageHandler(MSG_ID id, MSG_PARAM *pParam) { return 0; }

            // Static: the bodies in the DOL take no object.
            static void Initialize();
            static void Finalize();
            static CGameCamera *GetInstance(long view);
            static long GetInstanceNum();

            // One camera per view, made by each sport's Initialize.
            static CGameCamera *g_pInstance[4];
            static long g_nInstanceNum;

        protected:
            // 0xFC: flags. With 0x80 set the camera stands still
            // (GetFrameRateF returns 0). Meaning of the other bits not known.
            unsigned long m_UnkFC;

        private:
            unsigned char m_Unk100[0x254 - 0x100];
    };
}

#endif /* !CGAMECAMERA_HPP_ */
