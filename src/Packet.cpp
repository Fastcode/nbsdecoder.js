
#include "Packet.hpp"

#include <stdexcept>

namespace nbs {

    Packet Packet::FromJsValue(const Napi::Value& jsPacket, const Napi::Env& env) {

        if (!jsPacket.IsObject()) {
            throw std::runtime_error("expected packet object");
        }

        auto jsObject = jsPacket.As<Napi::Object>();

        // Check keys are valid
        if (!jsObject.Has("timestamp")) {
            throw std::runtime_error("expected object with `timestamp` key");
        }
        if (!jsObject.Has("type")) {
            throw std::runtime_error("expected object with `type` key");
        }
        if (!jsObject.Has("payload")) {
            throw std::runtime_error("expected object with `payload` key");
        }

        uint32_t subtype = 0;
        if (jsObject.Has("subtype")) {
            if (jsObject.Get("subtype").IsNumber()) {
                subtype = jsObject.Get("subtype").As<Napi::Number>().Uint32Value();
            }
            else if (!jsObject.Get("subtype").IsUndefined()) {
                throw std::runtime_error("expected `subtype` to be a number");
            }
        }

        uint64_t timestamp = 0;
        try {
            timestamp = timestamp::FromJsValue(jsObject.Get("timestamp"), env);
        }
        catch (const std::exception& ex) {
            throw std::runtime_error(std::string("error in `timestamp`: ") + ex.what());
        }

        uint64_t type = 0;
        try {
            type = hash::FromJsValue(jsObject.Get("type"), env);
        }
        catch (const std::exception& ex) {
            throw std::runtime_error(std::string("error in `type`: ") + ex.what());
        }

        if (!jsObject.Get("payload").IsBuffer()) {
            throw std::runtime_error("expected `payload` to be buffer object");
        }

        auto payloadBuffer = jsObject.Get("payload").As<Napi::Buffer<uint8_t>>();

        Packet packet;
        packet.timestamp = timestamp;
        packet.type      = type;
        packet.subtype   = subtype;
        packet.length    = payloadBuffer.ByteLength();
        packet.payload   = payloadBuffer.Data();  // Pointer to external JS managed memory

        return packet;
    }

    Napi::Value Packet::ToJsValue(const Packet& packet, const Napi::Env& env) {
        auto jsPacket = Napi::Object::New(env);

        jsPacket.Set("timestamp", timestamp::ToJsValue(packet.timestamp, env));
        jsPacket.Set("type", hash::ToJsValue(packet.type, env));
        jsPacket.Set("subtype", Napi::Number::New(env, packet.subtype));

        if (packet.payload == nullptr) {
            jsPacket.Set("payload", env.Undefined());
        }
        else {
            jsPacket.Set("payload", Napi::Buffer<uint8_t>::Copy(env, packet.payload, packet.length));
        }

        return jsPacket;
    }

}  // namespace nbs
