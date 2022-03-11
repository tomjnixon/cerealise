#pragma once

namespace userial {
template <typename T> struct Adapter {};

template <> struct Adapter<uint8_t> {
  template <typename T, typename F> static bool adapt(T v, F f) {
    return f.byte(v);
  }
};

template <> struct Adapter<uint32_t> {
  template <typename T, typename F> static bool adapt(T v, F f) {
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

  bool u32(uint32_t &x) {
    uint8_t buf[4];
    if (!bytes(buf, 4))
      return false;
    x = (uint32_t)(buf[0]) << 24 | (uint32_t)(buf[1]) << 16 |
        (uint32_t)(buf[2]) << 8 | (uint32_t)(buf[3]);
    return true;
  }

  template <typename T> bool operator()(T &x) {
    return Adapter<T>::template adapt<T &, ParseBuf &>(x, *this);
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

  bool u32(const uint32_t &x) {
    uint8_t buf[4];
    for (size_t i = 0; i < 4; i++) {
      buf[i] = (uint8_t)((x >> ((3 - i) * 8)) & 0xff);
    }
    return bytes(buf, 4);
  }

  template <typename T> bool operator()(const T &x) {
    return Adapter<T>::template adapt<const T &, UnparseBuf &>(x, *this);
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
} // namespace userial
