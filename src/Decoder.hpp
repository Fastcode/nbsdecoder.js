#ifndef NBS_DECODER_HPP
#define NBS_DECODER_HPP

#include <napi.h>
#include <vector>

#include "Index.hpp"
#include "Packet.hpp"
#include "TypeSubtype.hpp"
#include "mio/mmap.hpp"

namespace nbs {
    class Decoder : public Napi::ObjectWrap<Decoder> {
    public:
        /// Initialize the Decoder class NAPI binding
        static Napi::Object Init(Napi::Env& env, Napi::Object& exports);

        /// Constructor: takes a list of file paths from JS and constructs a Decoder
        Decoder(const Napi::CallbackInfo& info);

        /// Get a list of the available types in the nbs files of this decoder
        /// Returns a JS array of type subtype objects
        Napi::Value GetAvailableTypes(const Napi::CallbackInfo& info);

        /// Get a list of the available types in the nbs files of this decoder
        /// Returns a JS array with two elements: the start timestamp object and the end timestamp object
        Napi::Value GetTimestampRange(const Napi::CallbackInfo& info);

        /// Get a list of packets at the given timestamp matching the given list of types and subtypes
        /// Returns a JS array of packet objects
        Napi::Value GetPackets(const Napi::CallbackInfo& info);

    private:
        /// Holds the index for the nbs files loaded in this decoder
        Index index;

        /// Memory maps for each nbs file in the index, indexed by the file order in the list
        /// of file paths used to construct this decoder
        std::vector<mio::basic_mmap_source<uint8_t>> memoryMaps;

        /// Get the list of packets at the given timestamp matching the given list of types and subtypes
        std::vector<Packet> GetMatchingPackets(const uint64_t& timestamp, const std::vector<TypeSubtype>& types);

        /// Read the packet for the given index item
        Packet Read(const IndexItemFile& item);

        /// Create a XX64 hash from the given JS value, which could be a string or a buffer.
        /// String values will be hashed, and buffers will be interpreted as a XX64 hash.
        uint64_t HashFromJsValue(const Napi::Value& jsHash, const Napi::Env& env);

        /// Convert the given XX64 hash to a JS Buffer value
        Napi::Value HashToJsValue(const uint64_t& hash, const Napi::Env& env);

        /// Convert the given JS value to a timestamp in nanoseconds.
        /// The JS value can be a number, BigInt, or an object with `seconds` and `nanos` properties.
        uint64_t TimestampFromJsValue(const Napi::Value& jsTimestamp, const Napi::Env& env);

        /// Convert the given timestamp to a JS object with `seconds` and `nanos` properties
        Napi::Value TimestampToJsValue(const uint64_t& timestamp, const Napi::Env& env);

        /// Convert the given JS object with `type` and `subtype` properties to a TypeSubtype struct
        TypeSubtype TypeSubtypeFromJsValue(const Napi::Value& jsTypeSubtype, const Napi::Env& env);
    };

}  // namespace nbs

#endif  // NBS_DECODER_HPP
