#ifndef CTOURNAMENTCAMERA_HPP_
#define CTOURNAMENTCAMERA_HPP_

#include "game/msw/CInterpLockCamera.hpp"
#include "game/msw/TournamentData.hpp"

namespace msw {

    // Size 0x19C. The camera of the tournament map.
    class CTournamentCamera : public CInterpLockCamera {
        public:
            CTournamentCamera(long view, short priority);
            virtual ~CTournamentCamera() {}
            virtual void MainCamera();
            virtual int OnActive();

            // Moves to `rInfo` as seen from `rPos`, and stops following.
            void SetValueCamera(const TournamentData::CAMERA_INFO &rInfo, const bcs::FVEC3 &rPos);
            void SetValueCamera(const _CAMERA_INFO_LOOK &rInfo);

            // 1 when the view is of aspect type 1. Static: the callers pass
            // no object. Return type guessed.
            static int GetDataMode();

        private:
            unsigned char m_Unk184[0x8]; // 0x184
            bcs::FVEC3 m_Offset;         // 0x18C: added to *m_pTarget (name guessed)
            const bcs::FVEC3 *m_pTarget; // 0x198: the point followed, or null (name guessed;
                                         // stored by msw::CTournamentMan::SetCameraTarget)
    };
}

#endif /* !CTOURNAMENTCAMERA_HPP_ */
