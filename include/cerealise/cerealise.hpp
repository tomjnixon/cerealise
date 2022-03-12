#pragma once
#include <algorithm>
#include <bit>
#include <cstdint>
#include <type_traits>

namespace cerealise {

template <typename T, class Enable = void> struct Adapter {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    return T::template cerealise<TT, F>(v, f);
  }
};

template <typename T>
struct Adapter<T, std::enable_if_t<std::is_integral_v<T>>> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    return f.fixedint(v);
  }
};

template <typename T>
struct Adapter<T, std::enable_if_t<std::is_floating_point_v<T>>> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    return f.native(v);
  }
};

template <> struct Adapter<bool> {
  template <typename TT, typename F> static bool adapt(TT &v, F &f) {
    return f.boolean(v);
  }
};

namespace detail {

constexpr bool do_byte_swap = std::endian::native != std::endian::little;

template <typename Signed, typename Unsigned = std::make_unsigned_t<Signed>>
Unsigned encode_zigzag(Signed x) {
  return (Unsigned)(x << 1) ^ (Unsigned)(x >> (sizeof(Signed) * 8 - 1));
}

template <typename Unsigned, typename Signed = std::make_signed_t<Unsigned>>
Signed decode_zigzag(Unsigned x) {
  Signed y = (Signed)(x >> 1);
  return (x & 1) ? ~y : y;
}

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

  bool boolean(bool &x) {
    uint8_t b;
    if (!byte(b))
      return false;
    x = b ? true : false;
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

  template <typename T> bool native(T &x) {
    if constexpr (do_byte_swap) {
      uint8_t buf[sizeof(T)];
      if (!bytes(buf, sizeof(T)))
        return false;

      for (size_t i = 0; i < sizeof(T) / 2; i++) {
        size_t j = sizeof(T) - i;
        std::swap(buf[i], buf[j]);
      }

      x = *(T *)&buf;
      return true;
    } else
      return bytes((uint8_t *)&x, sizeof(T));
  }

  template <typename T> bool varint(T &x) {
    using uint = std::make_unsigned_t<T>;

    uint u = 0;
    uint8_t b;
    do {
      const uint high_7 = 0x7f << (sizeof(T) * 8 - 7);
      if (high_7 & u)
        return false; // ran out of space

      if (!byte(b))
        return false;

      u = (u << 7) | (b & 0x7f);
    } while (b & 0x80);

    if constexpr (std::is_signed_v<T>)
      x = decode_zigzag(u);
    else
      x = u;

    return true;
  }

  template <typename T> bool operator()(T &x) {
    return Adapter<std::remove_cv_t<T>>::template adapt<T, ParseBuf>(x, *this);
  }

  size_t bytes_read() const { return pos; }

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

  bool boolean(const bool &x) { return byte(x ? 1 : 0); }

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

  template <typename T> bool native(const T &x) {
    uint8_t *x_ptr = (uint8_t *)&x;

    if constexpr (do_byte_swap) {
      uint8_t buf[sizeof(T)];

      std::copy(x_ptr, x_ptr + sizeof(T), buf);

      for (size_t i = 0; i < sizeof(T) / 2; i++) {
        size_t j = sizeof(T) - i;
        std::swap(buf[i], buf[j]);
      }

      return bytes(buf, sizeof(T));
    } else
      return bytes(x_ptr, sizeof(T));
  }

  template <typename T> bool varint(const T &x) {
    using U = std::make_unsigned_t<T>;

    U u;
    if constexpr (std::is_signed_v<T>)
      u = encode_zigzag(x);
    else
      u = x;

    // find required number of bytes
    U remainder = u;
    size_t bytes = 0;
    do {
      remainder >>= 7;
      bytes += 1;
    } while (remainder > 0);

    for (int byteno = (int)bytes - 1; byteno >= 0; byteno--) {
      uint8_t b = (u >> (byteno * 7)) & 0x7f;
      if (byteno > 0)
        b |= 0x80;
      if (!byte(b))
        return false;
    }
    return true;
  }

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

} // namespace detail

template <typename T>
inline bool parse(T &v, uint8_t *buf, size_t buf_len, size_t &bytes_read) {
  detail::ParseBuf pb(buf, buf_len);

  bool res = pb(v);
  bytes_read = pb.bytes_read();
  return res;
}
template <typename T>
inline bool unparse(const T &v, uint8_t *buf, size_t buf_len,
                    size_t &bytes_written) {
  detail::UnparseBuf pb(buf, buf_len);

  bool res = pb(v);
  bytes_written = pb.bytes_written();
  return res;
}
} // namespace cerealise
