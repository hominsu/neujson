//
// Created by Homing So on 2022/3/10.
//

#ifndef NEUJSON_NEUJSON_WRITER_H_
#define NEUJSON_NEUJSON_WRITER_H_

#include <cmath>
#include <cstdint>

#include <algorithm>
#include <vector>

#include "internal/ieee754.h"
#include "internal/itoa.h"
#include "neujson.h"
#include "non_copyable.h"
#include "value.h"

#if defined(__GNUC__)
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(effc++)
#endif

namespace neujson {

template<typename WriteStream>
class Writer : NonCopyable {
 private:
  struct Level {
    bool in_array_flag_; // in array or object
    int value_count_;

    explicit Level(bool _in_array_flag) : in_array_flag_(_in_array_flag), value_count_(0) {}
  };

 protected:
  std::vector<Level> stack_;
  WriteStream &os_;
  bool has_root_;

 public:
  explicit Writer(WriteStream &_os) : os_(_os), has_root_(false) {}
  virtual ~Writer() = default;
  
  virtual bool Null() {
    Prefix(NEU_NULL);
    return EndValue(WriteNull());
  }
  virtual bool Bool(bool _b) {
    Prefix(NEU_BOOL);
    return EndValue(WriteBool(_b));
  }
  virtual bool Int32(int32_t _i32) {
    Prefix(NEU_INT32);
    return EndValue(WriteInt32(_i32));
  }
  virtual bool Int64(int64_t _i64) {
    Prefix(NEU_INT64);
    return EndValue(WriteInt64(_i64));
  }
  virtual bool Double(double _d) {
    Prefix(NEU_DOUBLE);
    return EndValue(WriteDouble(internal::Double(_d)));
  }
  virtual bool Double(internal::Double _d) {
    Prefix(NEU_DOUBLE);
    return EndValue(WriteDouble(_d));
  }
  virtual bool String(std::string_view _str) {
    Prefix(NEU_STRING);
    return EndValue(WriteString(_str));
  }
  virtual bool Key(std::string_view _str) {
    Prefix(NEU_STRING);
    return EndValue(WriteKey(_str));
  }
  virtual bool StartObject() {
    Prefix(NEU_OBJECT);
    stack_.emplace_back(false);
    return EndValue(WriteStartObject());
  }
  virtual bool EndObject() {
    NEUJSON_ASSERT(!stack_.empty());
    NEUJSON_ASSERT(!stack_.back().in_array_flag_);
    stack_.pop_back();
    return EndValue(WriteEndObject());
  }
  virtual bool StartArray() {
    Prefix(NEU_ARRAY);
    stack_.emplace_back(true);
    return EndValue(WriteStartArray());
  }
  virtual bool EndArray() {
    NEUJSON_ASSERT(!stack_.empty());
    NEUJSON_ASSERT(stack_.back().in_array_flag_);
    stack_.pop_back();
    return EndValue(WriteEndArray());
  }

 protected:
  void Prefix(Type type_);

  bool WriteNull();
  bool WriteBool(bool _b);
  bool WriteInt32(int32_t _i32);
  bool WriteInt64(int64_t _i64);
  bool WriteDouble(internal::Double _d);
  bool WriteString(std::string_view _str);
  bool WriteKey(std::string_view _str);
  bool WriteStartObject();
  bool WriteEndObject();
  bool WriteStartArray();
  bool WriteEndArray();
  bool EndValue(bool _ret);
  void Flush() { os_.flush(); }
};

template<typename WriteStream>
inline void Writer<WriteStream>::Prefix(Type type_) {
  (void) type_;
  if (!stack_.empty()) {
    auto &level = stack_.back();
    if (level.value_count_ > 0) {
      if (level.in_array_flag_) { os_.put(','); }
      else { os_.put((level.value_count_ % 2 == 0) ? ',' : ':'); }
    }

    if (!level.in_array_flag_ && level.value_count_ % 2 == 0) {
      NEUJSON_ASSERT(type_ == NEU_STRING && "miss quotation mark");
    }
    level.value_count_++;
  } else {
    NEUJSON_ASSERT(!has_root_ && "root not singular");
    has_root_ = true;
  }
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteNull() {
  os_.puts("null", 4);
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteBool(bool _b) {
  os_.puts(_b ? "true" : "false", _b ? 4 : 5);
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteInt32(int32_t _i32) {
  char buf[16]{};
  auto size = static_cast<std::size_t>(internal::i32toa(_i32, buf) - buf);
  os_.puts(buf, size);
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteInt64(int64_t _i64) {
  char buf[32]{};
  auto size = static_cast<std::size_t>(internal::i64toa(_i64, buf) - buf);
  os_.puts(buf, size);
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteDouble(internal::Double _d) {
  char buf[32];
  std::size_t size = 0;
  if (_d.IsInf()) {
#if defined(_MSC_VER)
    strcpy_s(buf, "Infinity");
#else
    strcpy(buf, "Infinity");
#endif
    size += 8;
  } else if (_d.IsNan()) {
#if defined(_MSC_VER)
    strcpy_s(buf, "NaN");
#else
    strcpy(buf, "NaN");
#endif
    size += 3;
  } else {
    int n = snprintf(buf, sizeof(buf), "%.17g", _d.Value());
    // type information loss if ".0" not added
    // "1.0" -> double 1 -> "1"
    NEUJSON_ASSERT(n > 0 && n < 32);
    size += static_cast<std::size_t>(n);
    if (std::find_if_not(buf, buf + n, isdigit) == buf + n) {
#if defined(_MSC_VER)
      strcat_s(buf, ".0");
#else
      strcat(buf, ".0");
#endif
      size += 2;
    }
  }

  os_.puts(buf, size);
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteString(std::string_view _str) {
  os_.put('"');
  for (auto c : _str) {
    auto u = static_cast<unsigned char>(c);
    switch (u) {
      case '\"': os_.puts("\\\"", 2);
        break;
      case '\b': os_.puts("\\b", 2);
        break;
      case '\f': os_.puts("\\f", 2);
        break;
      case '\n': os_.puts("\\n", 2);
        break;
      case '\r': os_.puts("\\r", 2);
        break;
      case '\t': os_.puts("\\t", 2);
        break;
      case '\\': os_.puts("\\\\", 2);
        break;
      default:
        if (u < 0x20) {
          char buf[7];
          snprintf(buf, 7, "\\u%04X", u);
          os_.puts(buf, 6);
        } else {
          os_.put(c);
        }
    }
  }
  os_.put('"');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteKey(std::string_view _str) {
  WriteString(_str);
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteStartObject() {
  os_.put('{');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteEndObject() {
  os_.put('}');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteStartArray() {
  os_.put('[');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteEndArray() {
  os_.put(']');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::EndValue(bool _ret) {
  // end of json text
  if (stack_.empty()) { Flush(); }
  return _ret;
}

} // namespace neujson

#if defined(__GNUC__)
NEUJSON_DIAG_POP
#endif

#endif //NEUJSON_NEUJSON_WRITER_H_
