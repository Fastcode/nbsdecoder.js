#ifndef NBS_ENCODER_HPP
#define NBS_ENCODER_HPP

#include <fstream>
#include <napi.h>

#include "Packet.hpp"
#include "zstr/zstr.hpp"

namespace nbs {
    class Encoder : public Napi::ObjectWrap<Encoder> {
    public:
        /// Initialize Encoder class NAPI binding
        static Napi::Object Init(Napi::Env& env, Napi::Object& exports);

        Encoder(const Napi::CallbackInfo& info);

        Napi::Value Write(const Napi::CallbackInfo& info);

        Napi::Value GetBytesWritten(const Napi::CallbackInfo& info);

        void Close(const Napi::CallbackInfo& info);

        Napi::Value IsOpen(const Napi::CallbackInfo& info);

    private:
        /// The size of the radiation symbol at the start of each packet
        static constexpr int HEADER_SIZE = 3;
        /// The nbs file being written to
        std::ofstream output_file;
        /// The index file of the nbs file being written to
        zstr::ofstream index_file;
        /// The total number of bytes written to the nbs file so far
        uint64_t bytes_written{0};
    };
}  // namespace nbs

#endif  // NBS_ENCODER_HPP