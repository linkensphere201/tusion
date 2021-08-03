#pragma once
#include <cstdint>
#include <string>
#include <random>
#include <memory>

namespace tusion {
namespace util {
uint64_t NowMicros();

class Random {
  private:
    enum : uint32_t {
        M = 2147483647L // 2^31-1
    };
    enum : uint64_t {
        A = 16807 // bits 14, 8, 7, 5, 2, 1, 0
    };

    uint32_t seed_;

    static uint32_t GoodSeed(uint32_t s) { return (s & M) != 0 ? (s & M) : 1; }

  public:
    // This is the largest value that can be returned from Next()
    enum : uint32_t { kMaxNext = M };

    explicit Random(uint32_t s) : seed_(GoodSeed(s)) {}

    void Reset(uint32_t s) { seed_ = GoodSeed(s); }

    uint32_t Next() {
        // We are computing
        //       seed_ = (seed_ * A) % M,    where M = 2^31-1
        //
        // seed_ must not be zero or M, or else all subsequent computed values
        // will be zero or M respectively.  For all other values, seed_ will end
        // up cycling through every number in [1,M-1]
        uint64_t product = seed_ * A;

        // Compute (product % M) using the fact that ((x << 31) % M) == x.
        seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
        // The first reduction may overflow by 1 bit, so we may need to
        // repeat.  mod == M is not possible; using > allows the faster
        // sign-bit-based test.
        if (seed_ > M) {
            seed_ -= M;
        }
        return seed_;
    }

    // Returns a uniformly distributed value in the range [0..n-1]
    // REQUIRES: n > 0
    uint32_t Uniform(int n) { return Next() % n; }

    // Randomly returns true ~"1/n" of the time, and false otherwise.
    // REQUIRES: n > 0
    bool OneIn(int n) { return Uniform(n) == 0; }

    // "Optional" one-in-n, where 0 or negative always returns false
    // (may or may not consume a random value)
    bool OneInOpt(int n) { return n > 0 && OneIn(n); }

    // Returns random bool that is true for the given percentage of
    // calls on average. Zero or less is always false and 100 or more
    // is always true (may or may not consume a random value)
    bool PercentTrue(int percentage) {
        return static_cast<int>(Uniform(100)) < percentage;
    }

    // Skewed: pick "base" uniformly from range [0,max_log] and then
    // return "base" random bits.  The effect is to pick a number in the
    // range [0,2^max_log-1] with exponential bias towards smaller numbers.
    uint32_t Skewed(int max_log) { return Uniform(1 << Uniform(max_log + 1)); }

    // Returns a random string of length "len"
    std::string RandomString(int len);

    // Generates a random string of len bytes using human-readable characters
    std::string HumanReadableString(int len);
};

class BaseDistribution {
  public:
    BaseDistribution(unsigned int _min, unsigned int _max)
        : min_value_size_(_min), max_value_size_(_max) {}
    virtual ~BaseDistribution() {}

    unsigned int Generate() {
        auto val = Get();
        if (NeedTruncate()) {
            val = std::max(min_value_size_, val);
            val = std::min(max_value_size_, val);
        }
        return val;
    }

  private:
    virtual unsigned int Get() = 0;
    virtual bool         NeedTruncate() { return true; }
    unsigned int         min_value_size_;
    unsigned int         max_value_size_;
};

class FixedDistribution : public BaseDistribution {
  public:
    FixedDistribution(unsigned int size)
        : BaseDistribution(size, size), size_(size) {}

  private:
    virtual unsigned int Get() override { return size_; }
    virtual bool         NeedTruncate() override { return false; }
    unsigned int         size_;
};

class NormalDistribution : public BaseDistribution,
                           public std::normal_distribution<double> {
  public:
    NormalDistribution(unsigned int _min, unsigned int _max)
        : BaseDistribution(_min, _max),
          // 99.7% values within the range [min, max].
          std::normal_distribution<double>((double)(_min + _max) / 2.0 /*mean*/,
                                           (double)(_max - _min) /
                                               6.0 /*stddev*/),
          gen_(rd_()) {}

  private:
    virtual unsigned int Get() override {
        return static_cast<unsigned int>((*this)(gen_));
    }
    std::random_device rd_;
    std::mt19937       gen_;
};

class UniformDistribution : public BaseDistribution,
                            public std::uniform_int_distribution<unsigned int> {
  public:
    UniformDistribution(unsigned int _min, unsigned int _max)
        : BaseDistribution(_min, _max),
          std::uniform_int_distribution<unsigned int>(_min, _max), gen_(rd_()) {
    }

    virtual unsigned int Get() override { return (*this)(gen_); }
  private:
    virtual bool         NeedTruncate() override { return false; }
    std::random_device   rd_;
    std::mt19937         gen_;
};

void CompressibleString(Random *rnd, double compressed_fraction, int len,
                        std::string *dst);


// Helper for quickly generating random data.

enum DistributeType {
    kUniform,
    kNormal,
    kFixed
};

class RandomGenerator {
 private:
  std::string data_;
  unsigned int pos_;
  uint64_t     cnt_;
  std::unique_ptr<BaseDistribution> dist_;

 public:
  RandomGenerator() : cnt_(0) {
     // case kUniform:
     //   dist_.reset(new UniformDistribution(1048576, 10485760));
     //   break;
     // case kNormal:
     dist_.reset(new NormalDistribution(1048576, 10485760));
     //   break;
     // case kFixed:
     // default:
     //   dist_.reset(new FixedDistribution(1048576));
     // We use a limited amount of data over and over again and ensure
     // that it is larger than the compression window (32KB), and also
     // large enough to serve all typical value sizes we want to write.
    Random rnd((uint32_t)(NowMicros()));
    std::string piece;
    while (data_.size() < 104857600) {
        {
            piece.resize(4096);
            for (int i = 0; i < 4096; ++i) {
                piece[i] = static_cast<char>('a' + rnd.Uniform(26));
            }
        }
        data_.append(piece);
    }
    pos_ = 0;
  }

  void Generate(unsigned int len, std::string& val) {
    bool need_shuffle = false;
    if (pos_ + len > data_.size()) {
      pos_ = 0;
      need_shuffle = true;
    }
    pos_ += len;
    val = std::string(data_.data() + pos_ - len, len);
    cnt_++;
    if (need_shuffle) {
        Shuffle();
    }
  }

  void Shuffle() {
    std::string d;
    std::string piece;
    Random rnd((uint32_t)(NowMicros()));
    while (d.size() < 104857600) {
        {
            piece.resize(4096);
            for (int i = 0; i < 4096; ++i) {
                piece[i] = static_cast<char>('a' + rnd.Uniform(26));
            }
        }
        d.append(piece);
    }
    data_ = d;
  }

  void Generate(std::string& val) {
    auto len = dist_->Generate();
    Generate(len, val);
  }
};


} // namespace util
} // namespace tusion
