#pragma once
#include <cstdint>
#include <type_traits>

namespace cerealise {

template <typename T> struct Adapter {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    return T::template cerealise<TT, F>(v, f);
  }
};

template <> struct Adapter<uint8_t> {
  template <typename T, typename F> static bool adapt(T &v, F &f) {
    return f.byte(v);
  }
};

template <> struct Adapter<uint32_t> {
  template <typename T, typename F> static bool adapt(T &v, F &f) {
    return f.u32(v);
  }
};

class ParseBuf {
public:
  static constexpr bool parsing = true;

  ParseBuf(uint8_t *buf, size_t len) : buf(buf), len(len) {}

  bool byte(uint8_t &x) {
    if (pos >= len)
      return false;
    x = buf[pos++];
    return true;
  }

  bool bytes(uint8_t *p, size_t n) {
    for (size_t i = 0; i < n; i++) {
      if (!byte(p[i]))
        return false;
    }
    return true;
  }

  template <size_t size_p = 0, typename T> bool fixedint(T &x) {
    // allow overriding size with only one parameter
    constexpr size_t size = size_p == 0 ? sizeof(T) : size_p;
    static_assert(size <= sizeof(T), "data size must be less than type size");

    uint8_t buf[size];
    if (!bytes(buf, size))
      return false;

    x = (T)0;
    for (size_t i = 0; i < size; i++) {
      size_t byte = (size - 1) - i;
      x |= (T)(buf[i]) << (byte * 8);
    }

    // sign extend if necessary
    if constexpr (std::is_signed_v<T> && size < sizeof(T)) {
      T signbit = (T)1 << (size * 8 - 1);
      x = (x ^ signbit) - signbit;
    }
    return true;
  }

  bool u32(uint32_t &x) { return fixedint(x); }

  template <typename T> bool operator()(T &x) {
    return Adapter<std::remove_cv_t<T>>::template adapt<T, ParseBuf>(x, *this);
  }

private:
  uint8_t *buf;
  size_t len;
  size_t pos = 0;
};

class UnparseBuf {
public:
  static constexpr bool parsing = false;

  UnparseBuf(uint8_t *buf, size_t len) : buf(buf), len(len) {}

  bool byte(const uint8_t &x) {
    if (pos >= len)
      return false;
    buf[pos++] = x;
    return true;
  }

  bool bytes(const uint8_t *p, size_t n) {
    for (size_t i = 0; i < n; i++) {
      if (!byte(p[i]))
        return false;
    }
    return true;
  }

  template <size_t size_p = 0, typename T> bool fixedint(const T &x) {
    // allow overriding size with only one parameter
    constexpr size_t size = size_p == 0 ? sizeof(T) : size_p;
    static_assert(size <= sizeof(T), "data size must be less than type size");

    // TODO: check that x is within range

    uint8_t buf[size];
    for (size_t i = 0; i < size; i++) {
      size_t byte = (size - 1) - i;
      buf[i] = (uint8_t)((x >> (byte * 8)) & 0xff);
    }

    return bytes(buf, size);
  }

  bool u32(const uint32_t &x) { return fixedint(x); }

  template <typename T> bool operator()(const T &x) {
    return Adapter<std::remove_cv_t<T>>::template adapt<const T, UnparseBuf>(
        x, *this);
  }

  size_t bytes_written() const { return pos; }

private:
  uint8_t *buf;
  size_t len;
  size_t pos = 0;
};

template <typename T> inline bool parse(T &v, uint8_t *buf, size_t len) {
  ParseBuf pb(buf, len);

  return pb(v);
}
template <typename T>
inline bool unparse(const T &v, uint8_t *buf, size_t buf_len, size_t &msg_len) {
  UnparseBuf pb(buf, buf_len);

  bool res = pb(v);
  msg_len = pb.bytes_written();
  return res;
}
} // namespace cerealise
