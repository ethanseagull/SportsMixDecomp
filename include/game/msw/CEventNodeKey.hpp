#ifndef CEVENTNODEKEY_HPP_
#define CEVENTNODEKEY_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CEventNodeSimple.hpp"

namespace msw {

    // A node path made of key frames (DOL class, 0x694 bytes, vptr at 0x00,
    // vtable lbl_80487078; nothing names it: class, member and function
    // names guessed). Four tracks, the x, y and z of the position and the
    // rotation, each with up to 16 keys and a float data buffer.
    // msw::CEventModel::SetMoveLinear builds one inline; only what it uses
    // is declared.
    class CEventNodeKey : public CEventNodeBase {
        public:
            // One key of a track (0x14 bytes).
            struct KEY {
                float fFrame;
                float fValue;
                float fIn;
                float fOut;
                long nType;
            };

            // One track (0x15C bytes).
            class CTrack {
                public:
                    // (fn_801FBFD0) Appends a key.
                    void AddKey(const KEY &rKey);
                    // (fn_801FBE84) Appends nNum floats of pData to the
                    // track's data.
                    void AddData(const float *pData, long nNum);

                private:
                    unsigned char m_Unk00[0x15C];
            };

            // fn_801FC984.
            CEventNodeKey();
            // The virtuals, in the DOL (not used here).
            virtual ~CEventNodeKey();
            virtual bool IsEnable(long nId);
            virtual void GetResult(bcs::FVEC3 &pos, bcs::FVEC3 &rot, float fFrame, long nId);
            virtual void GetPos(bcs::FVEC3 &pos, float fFrame, long nId);
            virtual void GetResultVec(bcs::FVEC3 &pos, bcs::FVEC3 &rot, bcs::FVEC3 &vec, float fFrame, long nId);
            virtual float GetNumFrame() const;
            // fn_801FD2A0: empties the tracks.
            void Clear();
            // fn_801FD15C: prepares the tracks once all keys are in.
            void Setup();
            // fn_801FC90C: the four floats the rotation track keeps for
            // rotation rRot.
            static void _GetRotData(float *pOut, const bcs::FVEC3 &rRot);

            // Starts filling track nTrack, collecting its data in pBuf.
            void Begin(short nTrack, float *pBuf)
            {
                m_pBuf = pBuf;
                m_nBufNum = 0;
                m_nTrack = nTrack;
            }
            void AddKey(float fFrame, float fValue, float fIn, float fOut, long nType)
            {
                KEY key;
                key.fFrame = fFrame;
                key.fValue = fValue;
                key.fIn = fIn;
                key.fOut = fOut;
                key.nType = nType;
                GetTrack()->AddKey(key);
            }
            // Collects the rotation data of rRot.
            void AddRot(const bcs::FVEC3 &rRot)
            {
                float data[4];
                _GetRotData(data, rRot);
                m_pBuf[m_nBufNum + 0] = data[0];
                m_pBuf[m_nBufNum + 1] = data[1];
                m_pBuf[m_nBufNum + 2] = data[2];
                m_pBuf[m_nBufNum + 3] = data[3];
                m_nBufNum += 4;
            }
            // Hands the collected data to the track.
            void End() { GetTrack()->AddData(m_pBuf, m_nBufNum); }
            // Done with the buffer.
            void Finish()
            {
                m_pBuf = 0;
                m_nBufNum = 0;
                Setup();
            }

        private:
            CTrack *GetTrack()
            {
                switch (m_nTrack) {
                    case 0:
                        return &m_TrackX;
                    case 1:
                        return &m_TrackY;
                    case 2:
                        return &m_TrackZ;
                    default:
                        return &m_TrackRot;
                }
            }

            unsigned char m_Unk04[0x108]; // 0x04
            CTrack m_TrackX;              // 0x10C
            CTrack m_TrackY;              // 0x268
            CTrack m_TrackZ;              // 0x3C4
            CTrack m_TrackRot;            // 0x520
            float m_Unk67C[4];            // 0x67C
            float *m_pBuf;                // 0x68C: buffer of the track being filled
            short m_nBufNum;              // 0x690: floats in it
            short m_nTrack;               // 0x692: track being filled
    }; // size 0x694
}

#endif /* !CEVENTNODEKEY_HPP_ */
