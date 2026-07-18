#include "jarvis/util/Sha256.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace jarvis::util {
namespace {

constexpr std::array<std::uint32_t, 64> K {
    0x428a2f98U, 0x71374491U, 0xb5c0fbcfU, 0xe9b5dba5U, 0x3956c25bU, 0x59f111f1U, 0x923f82a4U, 0xab1c5ed5U,
    0xd807aa98U, 0x12835b01U, 0x243185beU, 0x550c7dc3U, 0x72be5d74U, 0x80deb1feU, 0x9bdc06a7U, 0xc19bf174U,
    0xe49b69c1U, 0xefbe4786U, 0x0fc19dc6U, 0x240ca1ccU, 0x2de92c6fU, 0x4a7484aaU, 0x5cb0a9dcU, 0x76f988daU,
    0x983e5152U, 0xa831c66dU, 0xb00327c8U, 0xbf597fc7U, 0xc6e00bf3U, 0xd5a79147U, 0x06ca6351U, 0x14292967U,
    0x27b70a85U, 0x2e1b2138U, 0x4d2c6dfcU, 0x53380d13U, 0x650a7354U, 0x766a0abbU, 0x81c2c92eU, 0x92722c85U,
    0xa2bfe8a1U, 0xa81a664bU, 0xc24b8b70U, 0xc76c51a3U, 0xd192e819U, 0xd6990624U, 0xf40e3585U, 0x106aa070U,
    0x19a4c116U, 0x1e376c08U, 0x2748774cU, 0x34b0bcb5U, 0x391c0cb3U, 0x4ed8aa4aU, 0x5b9cca4fU, 0x682e6ff3U,
    0x748f82eeU, 0x78a5636fU, 0x84c87814U, 0x8cc70208U, 0x90befffaU, 0xa4506cebU, 0xbef9a3f7U, 0xc67178f2U
};

std::uint32_t rotr(std::uint32_t value, std::uint32_t bits) {
    return (value >> bits) | (value << (32U - bits));
}

class Sha256 {
public:
    void update(const std::uint8_t* data, std::size_t length) {
        bitLength_ += static_cast<std::uint64_t>(length) * 8ULL;
        for (std::size_t i = 0; i < length; ++i) {
            buffer_[bufferLength_++] = data[i];
            if (bufferLength_ == 64) {
                transform(buffer_.data());
                bufferLength_ = 0;
            }
        }
    }

    std::array<std::uint8_t, 32> digest() {
        buffer_[bufferLength_++] = 0x80U;

        if (bufferLength_ > 56) {
            while (bufferLength_ < 64) {
                buffer_[bufferLength_++] = 0;
            }
            transform(buffer_.data());
            bufferLength_ = 0;
        }

        while (bufferLength_ < 56) {
            buffer_[bufferLength_++] = 0;
        }

        for (int i = 7; i >= 0; --i) {
            buffer_[bufferLength_++] = static_cast<std::uint8_t>((bitLength_ >> (i * 8)) & 0xffU);
        }
        transform(buffer_.data());

        std::array<std::uint8_t, 32> output {};
        for (std::size_t i = 0; i < state_.size(); ++i) {
            output[i * 4] = static_cast<std::uint8_t>((state_[i] >> 24) & 0xffU);
            output[i * 4 + 1] = static_cast<std::uint8_t>((state_[i] >> 16) & 0xffU);
            output[i * 4 + 2] = static_cast<std::uint8_t>((state_[i] >> 8) & 0xffU);
            output[i * 4 + 3] = static_cast<std::uint8_t>(state_[i] & 0xffU);
        }
        return output;
    }

private:
    void transform(const std::uint8_t* chunk) {
        std::array<std::uint32_t, 64> words {};
        for (std::size_t i = 0; i < 16; ++i) {
            words[i] = (static_cast<std::uint32_t>(chunk[i * 4]) << 24)
                | (static_cast<std::uint32_t>(chunk[i * 4 + 1]) << 16)
                | (static_cast<std::uint32_t>(chunk[i * 4 + 2]) << 8)
                | static_cast<std::uint32_t>(chunk[i * 4 + 3]);
        }

        for (std::size_t i = 16; i < 64; ++i) {
            const auto s0 = rotr(words[i - 15], 7) ^ rotr(words[i - 15], 18) ^ (words[i - 15] >> 3);
            const auto s1 = rotr(words[i - 2], 17) ^ rotr(words[i - 2], 19) ^ (words[i - 2] >> 10);
            words[i] = words[i - 16] + s0 + words[i - 7] + s1;
        }

        auto a = state_[0];
        auto b = state_[1];
        auto c = state_[2];
        auto d = state_[3];
        auto e = state_[4];
        auto f = state_[5];
        auto g = state_[6];
        auto h = state_[7];

        for (std::size_t i = 0; i < 64; ++i) {
            const auto s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            const auto ch = (e & f) ^ ((~e) & g);
            const auto temp1 = h + s1 + ch + K[i] + words[i];
            const auto s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            const auto maj = (a & b) ^ (a & c) ^ (b & c);
            const auto temp2 = s0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        state_[0] += a;
        state_[1] += b;
        state_[2] += c;
        state_[3] += d;
        state_[4] += e;
        state_[5] += f;
        state_[6] += g;
        state_[7] += h;
    }

    std::array<std::uint32_t, 8> state_ {
        0x6a09e667U,
        0xbb67ae85U,
        0x3c6ef372U,
        0xa54ff53aU,
        0x510e527fU,
        0x9b05688cU,
        0x1f83d9abU,
        0x5be0cd19U
    };
    std::array<std::uint8_t, 64> buffer_ {};
    std::size_t bufferLength_ = 0;
    std::uint64_t bitLength_ = 0;
};

std::string toHex(const std::array<std::uint8_t, 32>& digest) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (const auto byte : digest) {
        out << std::setw(2) << static_cast<int>(byte);
    }
    return out.str();
}

} // namespace

std::string sha256Hex(const std::string& data) {
    Sha256 sha;
    sha.update(reinterpret_cast<const std::uint8_t*>(data.data()), data.size());
    return toHex(sha.digest());
}

std::string sha256FileHex(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) {
        throw std::runtime_error("Unable to read file for SHA-256: " + path.string());
    }

    Sha256 sha;
    std::array<char, 8192> buffer {};
    while (input) {
        input.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
        const auto read = input.gcount();
        if (read > 0) {
            sha.update(reinterpret_cast<const std::uint8_t*>(buffer.data()), static_cast<std::size_t>(read));
        }
    }

    return toHex(sha.digest());
}

} // namespace jarvis::util

