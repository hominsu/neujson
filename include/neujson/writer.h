//
// Created by HominSu on 2022/3/10.
//

#ifndef NEUJSON_NEUJSON_WRITER_H_
#define NEUJSON_NEUJSON_WRITER_H_

#include "neujson.h"
#include "noncopyable.h"
#include "value.h"
#include "internal/itoa.h"

#include <cmath>

#include <vector>
#include <algorithm>

namespace neujson {

template<typename WriteStream>
class Writer : noncopyable {
 private:
  struct Level {
    bool in_array_flag_; // in array or object
    int value_count_;

    explicit Level(bool _in_array_flag) : in_array_flag_(_in_array_flag), value_count_(0) {}
  };

 protected:
  ::std::vector<Level> stack_;
  WriteStream &os_;
  bool has_root_;

 public:
  explicit Writer(WriteStream &_os) : os_(_os), has_root_(false) {}

 public:
  //@formatter:off
  virtual bool Null() { Prefix(NEU_NULL); return EndValue(WriteNull()); }
  virtual bool Bool(bool _b) { Prefix(NEU_BOOL); return EndValue(WriteBool(_b)); }
  virtual bool Int32(int32_t _i32) { Prefix(NEU_INT32); return EndValue(WriteInt32(_i32)); }
  virtual bool Int64(int64_t _i64) { Prefix(NEU_INT64); return EndValue(WriteInt64(_i64)); }
  virtual bool Double(double _d) { Prefix(NEU_DOUBLE); return EndValue(WriteDouble(_d)); }
  virtual bool String(::std::string_view _str) { Prefix(NEU_STRING); return EndValue(WriteString(_str)); }
  virtual bool Key(::std::string_view _str) { Prefix(NEU_STRING); return EndValue(WriteKey(_str)); }
  //@formatter:on

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
  bool WriteDouble(double _d);
  bool WriteString(::std::string_view _str);
  bool WriteKey(::std::string_view _str);
  bool WriteStartObject();
  bool WriteEndObject();
  bool WriteStartArray();
  bool WriteEndArray();
  bool EndValue(bool _ret);

  void Flush();
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
  os_.put("null");
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteBool(bool _b) {
  os_.put(_b ? "true" : "false");
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteInt32(int32_t _i32) {
  char buf[16]{};
  internal::i32toa(_i32, buf);
  os_.put(::std::string_view(buf, internal::CountDecimalDigit32(_i32)));
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteInt64(int64_t _i64) {
  char buf[32]{};
  internal::i64toa(_i64, buf);
  os_.put(::std::string_view(buf, internal::CountDecimalDigit64(_i64)));
  return true;
}

template<typename WriteStream>
bool Writer<WriteStream>::WriteDouble(double _d) {
  char buf[32];
  if (std::isinf(_d)) {
#if defined(_MSC_VER)
    strcpy_s(buf, "Infinity");
#else
    strcpy(buf, "Infinity");
#endif
  } else if (std::isnan(_d)) {
#if defined(_MSC_VER)
    strcpy_s(buf, "NaN");
#else
    strcpy(buf, "NaN");
#endif
  } else {
    int n = sprintf(buf, "%.17g", _d);
    // type information loss if ".0" not added
    // "1.0" -> double 1 -> "1"
    NEUJSON_ASSERT(n > 0 && n < 32);
    if (std::find_if_not(buf, buf + n, isdigit) == buf + n) {
#if defined(_MSC_VER)
      strcat_s(buf, ".0");
#else
      strcat(buf, ".0");
#endif
    }
  }

  os_.put(buf);
  return true;
}

template<typename WriteStream>
bool Writer<WriteStream>::WriteString(::std::string_view _str) {
  os_.put('"');
  for (auto c: _str) {
    auto u = static_cast<unsigned char>(c);
    switch (u) {
      case '\"': os_.put("\\\"");
        break;
      case '\b': os_.put("\\b");
        break;
      case '\f': os_.put("\\f");
        break;
      case '\n': os_.put("\\n");
        break;
      case '\r': os_.put("\\r");
        break;
      case '\t': os_.put("\\t");
        break;
      case '\\': os_.put("\\\\");
        break;
      default:
        if (u < 0x20) {
          char buf[7];
          snprintf(buf, 7, "\\u%04X", u);
          os_.put(buf);
        } else {
          os_.put(c);
        }
    }
  }
  os_.put('"');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::WriteKey(::std::string_view _str) {
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

template<typename WriteStream>
inline void Writer<WriteStream>::Flush() {
  os_.flush();
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_WRITER_H_
