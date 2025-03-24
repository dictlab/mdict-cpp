#pragma once

#include <array>
#ifdef _MSC_VER
#include <intrin.h>
#endif
#ifdef __GNUG__
#include <cpuid.h>
#endif

namespace cpu_features {

    enum Features : uint64_t {
        None = 0,
        SSE = 1 << 0,
        SSE2 = 1 << 1,
        SSE3 = 1 << 2,
        SSSE3 = 1 << 3,
        SSE4_1 = 1 << 4,
        SSE4_2 = 1 << 5,
        AVX = 1 << 6,
        AVX2 = 1 << 7,
        AVX512F = 1 << 8,
        AVX512PF = 1 << 9,
        AVX512ER = 1 << 10,
        AVX512CD = 1 << 11,
    };

    //--------------------------------------------------------------------------------------------------------

    namespace detail {
#ifdef _MSC_VER
        inline void cpuid(std::array<int, 4>& info, int level) {
            __cpuid(info.data(), level);
        }
#endif
#ifdef __GNUG__
        inline void cpuid(std::array<int, 4>& info, int level) {
            auto ptr = reinterpret_cast<unsigned int*>(info.data());
            if (level == 1) {
                __get_cpuid(level, &ptr[0], &ptr[1], &ptr[2], &ptr[3]);
            } else {
                __cpuid_count(level, 0, ptr[0], ptr[1], ptr[2], ptr[3]);
            }
        }
#endif

        inline Features get_features_impl() {
            std::array<int, 4> info = {0};
            cpuid(info, 0);
            int feature_levels = info[0];

            // Feature level 1 always exists
            cpuid(info, 1);
            std::underlying_type_t<Features> features = Features::None;
            if (info[3] & (1 << 25)) { features |= Features::SSE; }
            if (info[3] & (1 << 26)) { features |= Features::SSE2; }
            if (info[2] & (1 <<  0)) { features |= Features::SSE3; }
            if (info[2] & (1 <<  9)) { features |= Features::SSSE3; }
            if (info[2] & (1 << 19)) { features |= Features::SSE4_1; }
            if (info[2] & (1 << 20)) { features |= Features::SSE4_2; }
            if (info[2] & (1 << 28)) { features |= Features::AVX; }

            // Feature level 7
            if (feature_levels >= 7) {
                std::array<int, 4> info7 = {0};
                cpuid(info7, 7);
                if (info7[1] & (1 <<  5)) { features |= Features::AVX2; }
                if (info7[1] & (1 << 16)) { features |= Features::AVX512F; }
                if (info7[1] & (1 << 26)) { features |= Features::AVX512PF; }
                if (info7[1] & (1 << 27)) { features |= Features::AVX512ER; }
                if (info7[1] & (1 << 28)) { features |= Features::AVX512CD; }
            }

            return static_cast<Features>(features);
        }
    }

    //--------------------------------------------------------------------------------------------------------

    inline Features get_features() {
        static Features s_features = detail::get_features_impl();
        return s_features;
    }

}