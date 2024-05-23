//
// Created by Homing So on 2022/3/9.
//

#ifndef NEUJSON_NEUJSON_DOCUMENT_H_
#define NEUJSON_NEUJSON_DOCUMENT_H_

#include <cstdint>

#include <string_view>
#include <variant>
#include <vector>

#include "exception.h"
#include "internal/ieee754.h"
#include "neujson.h"
#include "reader.h"
#include "string_read_stream.h"
#include "value.h"

#ifdef __GNUC__
NEUJSON_DIAG_PUSH
NEUJSON_DIAG_OFF(effc++)
#endif // __GNUC__

namespace neujson {

class Document : public Value {
  struct Level {
    Value *value;
    int value_count;

    explicit Level(Value *value) : value(value), value_count(0) {}

    [[nodiscard]] Type type() const { return value->GetType(); }
    [[nodiscard]] Value *lastValue() const;
  };

  std::vector<Level> stack_;
  Value key_;
  bool see_value_ = false;

public:
  error::ParseError Parse(const char *json, size_t len);
  error::ParseError Parse(std::string_view json);

  template <required::read_stream::HasAllRequiredFunctions ReadStream>
  error::ParseError ParseStream(ReadStream &rs);

  // handler
  bool Null();
  bool Bool(bool b);
  bool Int32(int32_t i32);
  bool Int64(int64_t i64);
  bool Double(internal::Double d);
  bool String(std::string_view str);
  bool Key(std::string_view str);
  bool StartObject();
  bool EndObject();
  bool StartArray();
  bool EndArray();

private:
  Value *AddValue(Value &&value);
};

inline Value *Document::Level::lastValue() const {
  if (type() == NEU_ARRAY) {
    return &std::get<NEU_ARRAY_TYPE>(value->data_)->back();
  } else {
    return &std::get<NEU_OBJECT_TYPE>(value->data_)->back().value_;
  }
}

inline error::ParseError Document::Parse(const char *json, const size_t len) {
  return Parse(std::string_view(json, len));
}

inline error::ParseError Document::Parse(const std::string_view json) {
  StringReadStream string_read_stream(json);
  return ParseStream(string_read_stream);
}

template <required::read_stream::HasAllRequiredFunctions ReadStream>
error::ParseError Document::ParseStream(ReadStream &rs) {
  return Reader::Parse(rs, *this);
}

inline bool Document::Null() {
  AddValue(Value(NEU_NULL));
  return true;
}

inline bool Document::Bool(const bool b) {
  AddValue(Value(b));
  return true;
}

inline bool Document::Int32(const int32_t i32) {
  AddValue(Value(i32));
  return true;
}

inline bool Document::Int64(const int64_t i64) {
  AddValue(Value(i64));
  return true;
}

inline bool Document::Double(const internal::Double d) {
  AddValue(Value(d));
  return true;
}

inline bool Document::String(const std::string_view str) {
  AddValue(Value(str));
  return true;
}

inline bool Document::Key(const std::string_view str) {
  AddValue(Value(str));
  return true;
}

inline bool Document::StartObject() {
  auto value = AddValue(Value(NEU_OBJECT));
  stack_.emplace_back(value);
  return true;
}

inline bool Document::EndObject() {
  NEUJSON_ASSERT(!stack_.empty());
  NEUJSON_ASSERT(stack_.back().type() == NEU_OBJECT);
  stack_.pop_back();
  return true;
}

inline bool Document::StartArray() {
  auto value = AddValue(Value(NEU_ARRAY));
  stack_.emplace_back(value);
  return true;
}

inline bool Document::EndArray() {
  NEUJSON_ASSERT(!stack_.empty());
  NEUJSON_ASSERT(stack_.back().type() == NEU_ARRAY);
  stack_.pop_back();
  return true;
}

inline Value *Document::AddValue(Value &&value) {
  Type type = value.GetType();
  (void)type;
  if (see_value_) {
    NEUJSON_ASSERT(!stack_.empty() && "root not singular");
  } else {
    NEUJSON_ASSERT(type_ == NEU_NULL);
    see_value_ = true;
    type_ = value.type_;

    switch (type) {
    case NEU_NULL:
      break;
    case NEU_BOOL:
      data_ = std::get<NEU_BOOL_TYPE>(value.data_);
      break;
    case NEU_INT32:
      data_ = std::get<NEU_INT32_TYPE>(value.data_);
      break;
    case NEU_INT64:
      data_ = std::get<NEU_INT64_TYPE>(value.data_);
      break;
    case NEU_DOUBLE:
      data_ = std::get<NEU_DOUBLE_TYPE>(value.data_);
      break;
    case NEU_STRING:
      data_ = std::get<NEU_STRING_TYPE>(value.data_);
      std::get<NEU_STRING_TYPE>(value.data_) = nullptr;
      break;
    case NEU_ARRAY:
      data_ = std::get<NEU_ARRAY_TYPE>(value.data_);
      std::get<NEU_ARRAY_TYPE>(value.data_) = nullptr;
      break;
    case NEU_OBJECT:
      data_ = std::get<NEU_OBJECT_TYPE>(value.data_);
      std::get<NEU_OBJECT_TYPE>(value.data_) = nullptr;
      break;
    default:
      break;
    }
    value.type_ = NEU_NULL;
    return this;
  }

  auto &top = stack_.back();
  if (top.type() == NEU_ARRAY) {
    top.value->AddValue(std::move(value));
    top.value_count++;
    return top.lastValue();
  } else {
    NEUJSON_ASSERT(top.type() == NEU_OBJECT);
    if (top.value_count % 2 == 0) {
      NEUJSON_ASSERT(type == NEU_STRING && "miss quotation mark");
      key_ = std::move(value);
      top.value_count++;
      return &key_;
    } else {
      top.value->AddMember(std::move(key_), std::move(value));
      top.value_count++;
      return top.lastValue();
    }
  }
}

} // namespace neujson

#ifdef __GNUC__
NEUJSON_DIAG_POP
#endif // __GNUC__

#endif // NEUJSON_NEUJSON_DOCUMENT_H_
