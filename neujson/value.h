//
// Created by Homin Su on 2022/3/5.
//

#ifndef NEUJSON_NEUJSON_VALUE_H_
#define NEUJSON_NEUJSON_VALUE_H_

#include <cassert>
#include <cstring>

#include <atomic>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <variant>

namespace neujson {

enum Type {
  NEU_NULL, NEU_BOOL, NEU_INT32, NEU_INT64, NEU_DOUBLE, NEU_STRING, NEU_ARRAY, NEU_OBJECT
};

class Value;
struct Member;

using Data = ::std::variant<
    ::std::monostate,
    bool,
    int32_t,
    int64_t,
    double,
    ::std::shared_ptr<::std::vector<char>>,
    ::std::shared_ptr<::std::vector<Value>>,
    ::std::shared_ptr<::std::vector<Member>>
>;

class Document;

class Value {
 public:
  using MemberIterator = ::std::vector<Member>::iterator;
  using ConstMemberIterator = ::std::vector<Member>::const_iterator;

 private:
  friend class Document;

  using String = ::std::vector<char>;
  using Array = ::std::vector<Value>;
  using Object = ::std::vector<Member>;
  using StringWithSharedPtr = ::std::shared_ptr<String>;
  using ArrayWithSharedPtr = ::std::shared_ptr<Array>;
  using ObjectWithSharedPtr = ::std::shared_ptr<Object>;

 private:
  Type type_;
  Data data_;

 public:
  explicit Value(Type _type = NEU_NULL);
  explicit Value(bool _b) : type_(NEU_BOOL), data_(_b) {};
  explicit Value(int32_t _i32) : type_(NEU_INT32), data_(_i32) {};
  explicit Value(int64_t _i64) : type_(NEU_INT64), data_(_i64) {};
  explicit Value(double _d) : type_(NEU_DOUBLE), data_(_d) {};
  explicit Value(::std::string_view _s) : type_(NEU_STRING), data_(::std::make_shared<String>(_s.begin(), _s.end())) {};
  explicit Value(const char *_s) : type_(NEU_STRING), data_(::std::make_shared<String>(_s, _s + strlen(_s))) {};
  Value(const char *_s, ::std::size_t _len) : Value(::std::string_view(_s, _len)) {};
  Value(const Value &_val) = default;
  Value(Value &&_val) noexcept: type_(_val.type_), data_(::std::move(_val.data_)) {};
  ~Value() = default;

  [[nodiscard]] Type getType() const { return type_; };
  ::std::size_t getSize();

  [[nodiscard]] bool isNull() const { return type_ == NEU_NULL; }
  [[nodiscard]] bool isBool() const { return type_ == NEU_BOOL; }
  [[nodiscard]] bool isInt32() const { return type_ == NEU_INT32; }
  [[nodiscard]] bool isInt64() const { return type_ == NEU_INT64 || type_ == NEU_INT32; }
  [[nodiscard]] bool isDouble() const { return type_ == NEU_DOUBLE; }
  [[nodiscard]] bool isString() const { return type_ == NEU_STRING; }
  [[nodiscard]] bool isArray() const { return type_ == NEU_ARRAY; }
  [[nodiscard]] bool isObject() const { return type_ == NEU_OBJECT; }

  // getter
  [[nodiscard]] bool getBool() const {
    assert(type_ == NEU_BOOL);
    return ::std::get<bool>(data_);
  }
  [[nodiscard]] int32_t getInt32() const {
    assert(type_ == NEU_INT32);
    return ::std::get<int32_t>(data_);
  }
  [[nodiscard]] int64_t getInt64() const {
    assert(type_ == NEU_INT64 || type_ == NEU_INT32);
    return type_ == NEU_INT64 ? ::std::get<int64_t>(data_) : ::std::get<int32_t>(data_);
  }
  [[nodiscard]] double getDouble() const {
    assert(type_ == NEU_DOUBLE);
    return ::std::get<double>(data_);
  }
  [[nodiscard]] ::std::string_view getStringView() const {
    assert(type_ == NEU_STRING);
    auto s_ptr = ::std::get<StringWithSharedPtr>(data_);
    // Avoid repeating the return type from the declaration; use a braced initializer list instead
    return {s_ptr->data(), s_ptr->size()};
  }
  [[nodiscard]] ::std::string getString() const {
    return ::std::string(getStringView());
  }
  [[nodiscard]] const auto &getArray() const {
    assert(type_ == NEU_ARRAY);
    return ::std::get<ArrayWithSharedPtr>(data_);
  }
  [[nodiscard]] const auto &getObject() const {
    assert(type_ == NEU_OBJECT);
    return ::std::get<ObjectWithSharedPtr>(data_);
  }

  // setter
  Value &setNull() {
    this->~Value();
    return *new(this) Value(NEU_NULL);
  }
  Value &setBool(bool _b) {
    this->~Value();
    return *new(this) Value(_b);
  }
  Value &setInt32(int32_t _i32) {
    this->~Value();
    return *new(this) Value(_i32);
  }
  Value &getInt64(int64_t _i64) {
    this->~Value();
    return *new(this) Value(_i64);
  }
  Value &setDouble(double _d) {
    this->~Value();
    return *new(this) Value(_d);
  }
  Value &setString(::std::string_view _s) {
    this->~Value();
    return *new(this) Value(_s);
  }
  Value &setArray() {
    this->~Value();
    return *new(this) Value(NEU_ARRAY);
  }
  Value &setObject() {
    this->~Value();
    return *new(this) Value(NEU_OBJECT);
  }

  Value &operator=(const Value &_val);
  Value &operator=(Value &&_val) noexcept;

  Value &operator[](size_t _index);
  const Value &operator[](size_t _index) const;
  Value &operator[](::std::string_view _key);
  const Value &operator[](::std::string_view _key) const;

  template<typename T>
  Value &addValue(T &&_value);

  MemberIterator memberBegin();
  MemberIterator memberEnd();
  MemberIterator findMember(::std::string_view _key);

  [[nodiscard]] ConstMemberIterator MemberBegin() const;
  [[nodiscard]] ConstMemberIterator MemberEnd() const;
  [[nodiscard]] ConstMemberIterator findMember(::std::string_view _key) const;

  template<typename T>
  Value &addMember(const char *_key, T &&_value);
  Value &addMember(Value &&_key, Value &&_value);

  template<typename Handler>
  bool writeTo(Handler &_handler) const;
};

struct Member {
  Member(Value &&_key, Value &&_value) : key_(::std::move(_key)), value_(::std::move(_value)) {}
  Member(::std::string_view _key, Value &&_value) : key_(_key), value_(::std::move(_value)) {}

  Value key_;
  Value value_;
};

inline neujson::Value &neujson::Value::operator=(const neujson::Value &_val) {
  assert(this != &_val);
  type_ = _val.type_;
  data_ = _val.data_;
  return *this;
}

inline neujson::Value &neujson::Value::operator=(neujson::Value &&_val) noexcept {
  assert(this != &_val);
  type_ = _val.type_;
  data_ = ::std::move(_val.data_);
  _val.type_ = NEU_NULL;
  return *this;
}

inline neujson::Value &neujson::Value::operator[](size_t _index) {
  assert(type_ == NEU_ARRAY);
  return ::std::get<ArrayWithSharedPtr>(data_)->at(_index);
}

inline const neujson::Value &neujson::Value::operator[](size_t _index) const {
  assert(type_ == NEU_ARRAY);
  return ::std::get<ArrayWithSharedPtr>(data_)->at(_index);
}

inline neujson::Value &neujson::Value::operator[](::std::string_view _key) {
  assert(type_ == NEU_OBJECT);
  auto it = findMember(_key);
  if (it != ::std::get<ObjectWithSharedPtr>(data_)->end()) {
    return it->value_;
  }

  assert(false && "value no found");
  static Value fake(NEU_NULL);
  return fake;
}

inline const neujson::Value &neujson::Value::operator[](::std::string_view _key) const {
  assert(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this)[_key];
}

template<typename T>
inline Value &Value::addValue(T &&_value) {
  assert(type_ == NEU_ARRAY);
  auto a_ptr = ::std::get<ArrayWithSharedPtr>(data_);
  a_ptr->emplace_back(::std::forward<T>(_value));
  return a_ptr->back();
}

inline neujson::Value::MemberIterator neujson::Value::memberBegin() {
  assert(type_ == NEU_OBJECT);
  return ::std::get<ObjectWithSharedPtr>(data_)->begin();
}

inline neujson::Value::MemberIterator neujson::Value::memberEnd() {
  assert(type_ == NEU_OBJECT);
  return ::std::get<ObjectWithSharedPtr>(data_)->end();
}

inline neujson::Value::ConstMemberIterator neujson::Value::MemberBegin() const {
  assert(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this).memberBegin();
}

inline neujson::Value::ConstMemberIterator neujson::Value::MemberEnd() const {
  assert(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this).memberEnd();
}

inline neujson::Value::ConstMemberIterator neujson::Value::findMember(::std::string_view _key) const {
  assert(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this).findMember(_key);
}

template<typename T>
inline neujson::Value &neujson::Value::addMember(const char *_key, T &&_value) {
  return addMember(Value(_key), Value(::std::forward<T>(_value)));
}

inline neujson::Value &neujson::Value::addMember(neujson::Value &&_key, neujson::Value &&_value) {
  assert(type_ == NEU_OBJECT);
  assert(_key.type_ == NEU_STRING);
  assert(findMember(_key.getStringView()) == memberEnd());
  auto o_ptr = ::std::get<ObjectWithSharedPtr>(data_);
  o_ptr->emplace_back(::std::move(_key), ::std::move(_value));
  return o_ptr->back().value_;
}

#define CALL_HANDLER(_expr) do { if (!(_expr)) { return false; } } while(false)

template<typename Handler>
bool Value::writeTo(Handler &_handler) const {
  switch (type_) {
    case NEU_NULL:CALL_HANDLER(_handler.Null());
      break;
    case NEU_BOOL:CALL_HANDLER(_handler.Bool(::std::get<bool>(data_)));
      break;
    case NEU_INT32:CALL_HANDLER(_handler.Int32(::std::get<int32_t>(data_)));
      break;
    case NEU_INT64:CALL_HANDLER(_handler.Int64(::std::get<int64_t>(data_)));
      break;
    case NEU_DOUBLE:CALL_HANDLER(_handler.Double(::std::get<double>(data_)));
      break;
    case NEU_STRING:CALL_HANDLER(_handler.String(getStringView()));
      break;
    case NEU_ARRAY:CALL_HANDLER(_handler.StartArray());
      for (auto &val: *getArray()) {
        CALL_HANDLER(val.writeTo(_handler));
      }
      CALL_HANDLER(_handler.EndArray());
      break;
    case NEU_OBJECT:CALL_HANDLER(_handler.StartObject());
      for (auto &mem: *getObject()) {
        _handler.Key(mem.key_.getStringView());
        CALL_HANDLER(mem.value_.writeTo(_handler));
      }
      CALL_HANDLER(_handler.EndObject());
      break;
    default:assert(false && "bad type");
  }
  return true;
}

#undef CALL_HANDLER

} // namespace neujson

#endif //NEUJSON_NEUJSON_VALUE_H_
