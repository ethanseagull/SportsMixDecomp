#ifndef NW4R_UT_DVDFILESTREAM_HPP_
#define NW4R_UT_DVDFILESTREAM_HPP_

#include "nw4r/ut/RuntimeTypeInfo.h"

namespace nw4r {
    namespace ut {

        // Only what the game's code uses so far (added for DOL
        // game/bcs/CMovie.cpp). 0x70 bytes, vptr at 0x00 (the size from the
        // constructor's last store at 0x6E and the member after it in DOL
        // fn_8004EB14's class).
        //
        // The virtuals of NW4R's IOStream / FileStream are written here on
        // the one class (added for DOL game/bcs/EF/CResource.cpp), in their
        // vtable order: the slots bcs::EF::CResource::Load calls are 0x14
        // (Read), 0x38 (GetSizeAlign) and 0x40 (GetSize); the destructor
        // calls Close through slot 0x10. Parameter types of the slots nobody
        // calls are simplified.
        class DvdFileStream {
            public:
                DvdFileStream();
                // (the DOL's fn_803D51C0) Opens the file `pPath`; check
                // IsAvailable afterwards.
                explicit DvdFileStream(const char *pPath);

                virtual const detail::RuntimeTypeInfo *GetRuntimeTypeInfo() const; // 0x08
                virtual ~DvdFileStream();                                         // 0x0C
                virtual void Close();                                             // 0x10
                virtual long Read(void *pBuf, unsigned long size);                // 0x14
                virtual bool ReadAsync(void *pBuf, unsigned long size, void *pCallback, void *pArg); // 0x18
                virtual void Write(const void *pBuf, unsigned long size);         // 0x1C
                virtual bool WriteAsync(const void *pBuf, unsigned long size, void *pCallback, void *pArg); // 0x20
                virtual bool IsBusy() const;                                      // 0x24
                virtual bool CanAsync() const;                                    // 0x28
                virtual bool CanRead() const;                                     // 0x2C
                virtual bool CanWrite() const;                                    // 0x30
                virtual unsigned long GetOffsetAlign() const;                     // 0x34
                virtual unsigned long GetSizeAlign() const;                       // 0x38
                virtual unsigned long GetBufferAlign() const;                     // 0x3C
                virtual unsigned long GetSize() const;                            // 0x40
                // (the DOL's fn_803D5830) origin 0 is the start of the file.
                virtual void Seek(long offset, unsigned long origin);             // 0x44

                bool IsAvailable() const { return m_bAvailable; }

            private:
                bool m_bAvailable;            // 0x04
                unsigned char m_Unk05[0x6B];
        };
    }
}

#endif /* !NW4R_UT_DVDFILESTREAM_HPP_ */
