//
// Created by HominSu on 2022/3/10.
//

#ifndef NEUJSON_NEUJSON_WRITER_H_
#define NEUJSON_NEUJSON_WRITER_H_

#include "noncopyable.h"
#include "value.h"
#include "internal/itoa.h"

#include <cassert>

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

 private:
  ::std::vector<Level> stack_;
  WriteStream &os_;
  bool see_value_;

 public:
  explicit Writer(WriteStream &_os) : os_(_os), see_value_(false) {}

 private:
  void prefix(Type type_);

 public:
  bool Null();
  bool Bool(bool _b);
  bool Int32(int32_t _i32);
  bool Int64(int64_t _i64);
  bool Double(double _d);
  bool String(::std::string_view _str);
  bool Key(::std::string_view _str);
  bool StartObject();
  bool EndObject();
  bool StartArray();
  bool EndArray();
};

template<typename WriteStream>
void Writer<WriteStream>::prefix(Type type_) {
  if (see_value_) {
    assert(!stack_.empty() && "root not singular");
  } else {
    see_value_ = true;
  }

  if (stack_.empty()) { return; }

  auto &top = stack_.back();
  if (top.in_array_flag_) {
    if (top.value_count_ > 0) { os_.put(','); }
  } else {
    if (top.value_count_ % 2 == 1) {
      os_.put(':');
    } else {
      assert(type_ == NEU_STRING && "miss quotation mark");
      if (top.value_count_ > 0) {
        os_.put(',');
      }
    }
  }
  top.value_count_++;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::Null() {
  prefix(NEU_NULL);
  os_.put("null");
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::Bool(bool _b) {
  prefix(NEU_BOOL);
  os_.put(_b ? "true" : "false");
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::Int32(int32_t _i32) {
  prefix(NEU_INT32);
  char buf[16]{};
  internal::i32toa(_i32, buf);
  os_.put(::std::string_view(buf, internal::CountDecimalDigit32(_i32)));
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::Int64(int64_t _i64) {
  prefix(NEU_INT64);
  char buf[32]{};
  internal::i64toa(_i64, buf);
  os_.put(::std::string_view(buf, internal::CountDecimalDigit64(_i64)));
  return true;
}

template<typename WriteStream>
bool Writer<WriteStream>::Double(double _d) {
  prefix(NEU_DOUBLE);

  char buf[32];
  if (std::isinf(_d)) {
    strcpy(buf, "Infinity");
  } else if (std::isnan(_d)) {
    strcpy(buf, "NaN");
  } else {
    int n = sprintf(buf, "%.17g", _d);
    // type information loss if ".0" not added
    // "1.0" -> double 1 -> "1"
    assert(n > 0 && n < 32);
    auto it = std::find_if_not(buf, buf + n, isdigit);
    if (it == buf + n) {
      strcat(buf, ".0");
    }
  }

  os_.put(buf);
  return true;
}

template<typename WriteStream>
bool Writer<WriteStream>::String(::std::string_view _str) {
  prefix(NEU_STRING);
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
inline bool Writer<WriteStream>::Key(::std::string_view _str) {
  prefix(NEU_STRING);
  os_.put('"');
  os_.put(_str);
  os_.put('"');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::StartObject() {
  prefix(NEU_OBJECT);
  stack_.emplace_back(false);
  os_.put('{');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::EndObject() {
  assert(!stack_.empty());
  assert(!stack_.back().in_array_flag_);
  stack_.pop_back();
  os_.put('}');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::StartArray() {
  prefix(NEU_ARRAY);
  stack_.emplace_back(true);
  os_.put('[');
  return true;
}

template<typename WriteStream>
inline bool Writer<WriteStream>::EndArray() {
  assert(!stack_.empty());
  assert(stack_.back().in_array_flag_);
  stack_.pop_back();
  os_.put(']');
  return true;
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_WRITER_H_
