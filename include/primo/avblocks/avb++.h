#pragma once

#include <primo/avblocks/avb.h>
#include <primo/platform/reference++.h>
#include <primo/platform/ustring.h>

#include <string>
#include <memory>
#include <optional>
#include <stdexcept>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

namespace primo::avblocks::modern {

class TAVBlocksException : public std::runtime_error {
public:
    explicit TAVBlocksException(const std::string& message) 
        : std::runtime_error(message) {}
};

class TLibrary {
public:
    TLibrary() {
        primo::avblocks::Library::initialize();
    }
    
    ~TLibrary() {
        primo::avblocks::Library::shutdown();
    }
    
    TLibrary(const TLibrary&) = delete;
    TLibrary& operator=(const TLibrary&) = delete;
    TLibrary(TLibrary&&) = delete;
    TLibrary& operator=(TLibrary&&) = delete;
};

class TMediaSample {
    primo::ref<primo::codecs::MediaSample> sample_;
    
public:
    TMediaSample() 
        : sample_(primo::avblocks::Library::createMediaSample()) {}
    
    explicit TMediaSample(primo::ref<primo::codecs::MediaSample> sample)
        : sample_(std::move(sample)) {}
    
    // Delete copy operations
    TMediaSample(const TMediaSample&) = delete;
    TMediaSample& operator=(const TMediaSample&) = delete;
    
    // Enable move operations
    TMediaSample(TMediaSample&&) = default;
    TMediaSample& operator=(TMediaSample&&) = default;
    
    primo::codecs::MediaSample* get() const { return sample_.get(); }
};

class TAudioStreamInfo {
    primo::ref<primo::codecs::AudioStreamInfo> info_;
    
public:
    TAudioStreamInfo() 
        : info_(primo::avblocks::Library::createAudioStreamInfo()) {}
    
    explicit TAudioStreamInfo(primo::ref<primo::codecs::AudioStreamInfo> info)
        : info_(std::move(info)) {}
    
    // Delete copy operations
    TAudioStreamInfo(const TAudioStreamInfo&) = delete;
    TAudioStreamInfo& operator=(const TAudioStreamInfo&) = delete;
    
    // Enable move operations
    TAudioStreamInfo(TAudioStreamInfo&&) = default;
    TAudioStreamInfo& operator=(TAudioStreamInfo&&) = default;
    
    TAudioStreamInfo& streamType(primo::codecs::StreamType::Enum type) {
        info_->setStreamType(type);
        return *this;
    }
    
    TAudioStreamInfo& channels(int32_t channels) {
        info_->setChannels(channels);
        return *this;
    }
    
    TAudioStreamInfo& sampleRate(int32_t sampleRate) {
        info_->setSampleRate(sampleRate);
        return *this;
    }
    
    TAudioStreamInfo& bitsPerSample(int32_t bits) {
        info_->setBitsPerSample(bits);
        return *this;
    }
    
    primo::codecs::AudioStreamInfo* get() const { return info_.get(); }
};

class TMediaPin {
    primo::ref<primo::avblocks::MediaPin> pin_;
    
public:
    TMediaPin() 
        : pin_(primo::avblocks::Library::createMediaPin()) {}
    
    explicit TMediaPin(primo::ref<primo::avblocks::MediaPin> pin)
        : pin_(std::move(pin)) {}
    
    // Delete copy operations
    TMediaPin(const TMediaPin&) = delete;
    TMediaPin& operator=(const TMediaPin&) = delete;
    
    // Enable move operations
    TMediaPin(TMediaPin&&) = default;
    TMediaPin& operator=(TMediaPin&&) = default;
    
    TMediaPin&& streamInfo(const TAudioStreamInfo& info) && {
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    TMediaPin& streamInfo(const TAudioStreamInfo& info) & {
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    // Builder methods for audio pins
    TMediaPin&& audioStreamType(primo::codecs::StreamType::Enum streamType) && {
        primo::ref<primo::codecs::AudioStreamInfo> info(primo::avblocks::Library::createAudioStreamInfo());
        info->setStreamType(streamType);
        pin_->setStreamInfo(info.get());
        return std::move(*this);
    }
    
    TMediaPin& audioStreamType(primo::codecs::StreamType::Enum streamType) & {
        primo::ref<primo::codecs::AudioStreamInfo> info(primo::avblocks::Library::createAudioStreamInfo());
        info->setStreamType(streamType);
        pin_->setStreamInfo(info.get());
        return *this;
    }
    
    TMediaPin&& channels(int32_t channels) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setChannels(channels);
        }
        return std::move(*this);
    }
    
    TMediaPin& channels(int32_t channels) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setChannels(channels);
        }
        return *this;
    }
    
    TMediaPin&& sampleRate(int32_t sampleRate) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setSampleRate(sampleRate);
        }
        return std::move(*this);
    }
    
    TMediaPin& sampleRate(int32_t sampleRate) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setSampleRate(sampleRate);
        }
        return *this;
    }

    TMediaPin&& bitsPerSample(int32_t bits) && {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setBitsPerSample(bits);
        }
        return std::move(*this);
    }
    
    TMediaPin& bitsPerSample(int32_t bits) & {
        if (auto* info = dynamic_cast<primo::codecs::AudioStreamInfo*>(pin_->streamInfo())) {
            info->setBitsPerSample(bits);
        }
        return *this;
    }
    
    primo::avblocks::MediaPin* get() const { return pin_.get(); }
};

// String traits for handling both char and wchar_t
template<typename CharT>
struct string_traits;

template<>
struct string_traits<char> {
    using string_type = std::string;
    static primo::ustring to_ustring(const string_type& s) {
        return primo::ustring(s);
    }
};

template<>
struct string_traits<wchar_t> {
    using string_type = std::wstring;
    static primo::ustring to_ustring(const string_type& s) {
        return primo::ustring(s);
    }
};

template<typename CharT = char>
class TMediaSocketT {
    primo::ref<primo::avblocks::MediaSocket> socket_;
    
public:
    using string_type = typename string_traits<CharT>::string_type;
    
    TMediaSocketT() 
        : socket_(primo::avblocks::Library::createMediaSocket()) {}
    
    explicit TMediaSocketT(primo::ref<primo::avblocks::MediaSocket> socket)
        : socket_(std::move(socket)) {}
    
    // Delete copy operations
    TMediaSocketT(const TMediaSocketT&) = delete;
    TMediaSocketT& operator=(const TMediaSocketT&) = delete;
    
    // Enable move operations
    TMediaSocketT(TMediaSocketT&&) = default;
    TMediaSocketT& operator=(TMediaSocketT&&) = default;
    
    // Rvalue overloads for method chaining
    TMediaSocketT&& file(const string_type& path) && {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return std::move(*this);
    }
    
    TMediaSocketT& file(const string_type& path) & {
        socket_->setFile(string_traits<CharT>::to_ustring(path));
        return *this;
    }
    
    TMediaSocketT&& streamType(primo::codecs::StreamType::Enum type) && {
        socket_->setStreamType(type);
        return std::move(*this);
    }
    
    TMediaSocketT& streamType(primo::codecs::StreamType::Enum type) & {
        socket_->setStreamType(type);
        return *this;
    }
    
    TMediaSocketT&& addPin(TMediaPin&& pin) && {
        socket_->pins()->add(pin.get());
        return std::move(*this);
    }
    
    TMediaSocketT& addPin(TMediaPin&& pin) & {
        socket_->pins()->add(pin.get());
        return *this;
    }
    
    primo::avblocks::MediaSocket* get() const { return socket_.get(); }
};

// Type aliases for convenience
using TMediaSocket = TMediaSocketT<char>;
using TMediaSocketW = TMediaSocketT<wchar_t>;

template<typename CharT = char>
class TTranscoderT {
    primo::ref<primo::avblocks::Transcoder> transcoder_;
    
public:
    TTranscoderT() 
        : transcoder_(primo::avblocks::Library::createTranscoder()) {}
    
    // Delete copy operations
    TTranscoderT(const TTranscoderT&) = delete;
    TTranscoderT& operator=(const TTranscoderT&) = delete;
    
    // Enable move operations
    TTranscoderT(TTranscoderT&&) = default;
    TTranscoderT& operator=(TTranscoderT&&) = default;
    
    TTranscoderT& allowDemoMode(bool allow = true) {
        transcoder_->setAllowDemoMode(allow ? TRUE : FALSE);
        return *this;
    }
    
    TTranscoderT& addInput(const TMediaSocketT<CharT>& socket) {
        transcoder_->inputs()->add(socket.get());
        return *this;
    }
    
    TTranscoderT& addOutput(const TMediaSocketT<CharT>& socket) {
        transcoder_->outputs()->add(socket.get());
        return *this;
    }
    
    TTranscoderT& open() {
        if (!transcoder_->open()) {
            auto* error = transcoder_->error();
            std::string msg = "Failed to open transcoder";
            if (error && error->message()) {
                msg += ": " + std::string(primo::ustring(error->message()));
            }
            throw TAVBlocksException(msg);
        }
        return *this;
    }
    
    bool pull(int32_t& outputIndex, TMediaSample& sample) {
        return transcoder_->pull(outputIndex, sample.get()) == TRUE;
    }

    bool push(int32_t inputIndex, TMediaSample& sample) {
        return transcoder_->push(inputIndex, sample.get()) == TRUE;
    }
    
    TTranscoderT& run() {
        if (!transcoder_->run()) {
            auto* error = transcoder_->error();
            std::string msg = "Failed to run transcoder";
            if (error && error->message()) {
                msg += ": " + std::string(primo::ustring(error->message()));
            }
            throw TAVBlocksException(msg);
        }
        return *this;
    }
    
    void close() {
        transcoder_->close();
    }

    const primo::error::ErrorInfo* error() const {
        return transcoder_->error();
    }
    
    primo::avblocks::Transcoder* get() const { return transcoder_.get(); }
};

// Type aliases for convenience
using TTranscoder = TTranscoderT<char>;
using TTranscoderW = TTranscoderT<wchar_t>;

} // namespace primo::avblocks::modern