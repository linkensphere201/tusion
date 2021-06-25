#include "util.h"

#include <sys/time.h>

namespace tusion {
namespace util {
uint64_t NowMicros() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return static_cast<uint64_t>(tv.tv_sec) * 1000000 + tv.tv_usec;
}
} // namespace util
} // namespace tusion
