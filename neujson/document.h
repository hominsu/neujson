//
// Created by Homin Su on 2022/3/9.
//

#ifndef NEUJSON_NEUJSON_DOCUMENT_H_
#define NEUJSON_NEUJSON_DOCUMENT_H_

#include <neujson/exception.h>
#include <neujson/value.h>
#include <neujson/reader.h>
#include <neujson/string_read_stream.h>

namespace neujson {

class Document : public Value {
 private:
  struct Level {
    Value *value;
    int valueCount;

    explicit Level(Value *value_) : value(value_), valueCount(0) {}

    [[nodiscard]] Type type() const { return value->getType(); }
    [[nodiscard]] Value *lastValue();
  };

 private:
  std::vector<Level> stack_;
  Value key_;
  bool seeValue_ = false;

 public:
  error::ParseError parse(const char *_json, size_t _len);
  error::ParseError parse(std::string_view _json);

  template<typename ReadStream>
  error::ParseError parseStream(ReadStream &_rs);

 public: // handler
  bool Null();
  bool Bool(bool _b);
  bool Int32(int32_t _i32);
  bool Int64(int64_t _i64);
  bool Double(double _d);
  bool String(std::string_view _string_view);
  bool Key(std::string_view _string_view);
  bool StartObject();
  bool EndObject();
  bool StartArray();
  bool EndArray();

 private:
  Value *addValue(Value &&_value);
};

inline Value *Document::Level::lastValue() {
  if (type() == NEU_ARRAY) {
    return &::std::get<Value::ArrayWithSharedPtr>(value->data_)->back();
  } else {
    return &::std::get<Value::ObjectWithSharedPtr>(value->data_)->back().value_;
  }
}

inline error::ParseError Document::parse(const char *_json, size_t _len) {
  return parse(std::string_view(_json, _len));
}

inline error::ParseError Document::parse(std::string_view _json) {
  StringReadStream string_read_stream(_json);
  return parseStream(string_read_stream);
}

template<typename ReadStream>
inline error::ParseError Document::parseStream(ReadStream &_rs) {
  return Reader::parse(_rs, *this);
}

inline bool Document::Null() {
  addValue(Value(NEU_NULL));
  return true;
}

inline bool Document::Bool(bool _b) {
  addValue(Value(_b));
  return true;
}

inline bool Document::Int32(int32_t _i32) {
  addValue(Value(_i32));
  return true;
}

inline bool Document::Int64(int64_t _i64) {
  addValue(Value(_i64));
  return true;
}

inline bool Document::Double(double _d) {
  addValue(Value(_d));
  return true;
}

inline bool Document::String(std::string_view _string_view) {
  addValue(Value(_string_view));
  return true;
}

inline bool Document::Key(std::string_view _string_view) {
  addValue(Value(_string_view));
  return true;
}

inline bool Document::StartObject() {
  auto value = addValue(Value(NEU_OBJECT));
  stack_.emplace_back(value);
  return true;
}

inline bool Document::EndObject() {
  assert(!stack_.empty());
  assert(stack_.back().type() == NEU_OBJECT);
  stack_.pop_back();
  return true;
}

inline bool Document::StartArray() {
  auto value = addValue(Value(NEU_ARRAY));
  stack_.emplace_back(value);
  return true;
}

inline bool Document::EndArray() {
  assert(!stack_.empty());
  assert(stack_.back().type() == NEU_ARRAY);
  stack_.pop_back();
  return true;
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_DOCUMENT_H_
