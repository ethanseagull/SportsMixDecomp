#ifndef NW4R_SND_BASICSOUND_HPP_
#define NW4R_SND_BASICSOUND_HPP_

namespace nw4r {
    namespace snd {
        namespace detail {

            // A sound of the DOL's NW4R library. Minimal declarations written
            // for this project: the signatures are the DOL's mangled names,
            // the virtual slot of IsPrepared is the one bcs::CSndTask calls
            // (vtable offset 0x14). The other virtuals are placeholders that
            // only give it that slot.
            class BasicSound {
                public:
                    virtual void Vf08();
                    virtual void Vf0C();
                    virtual void Vf10();
                    virtual bool IsPrepared() const;

                    void StartPrepared();
                    void Stop(int fadeFrames);
                    void Pause(bool bPause, int fadeFrames);
                    void SetVolume(float volume, int frames);
                    // fn_803BA1D0 (stores the pitch at 0xBC)
                    void SetPitch(float pitch);
            };
        }
    }
}

#endif /* !NW4R_SND_BASICSOUND_HPP_ */
