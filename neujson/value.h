//
// Created by Homin Su on 2022/3/5.
//

#ifndef NEUJSON_NEUJSON_VALUE_H_
#define NEUJSON_NEUJSON_VALUE_H_

#include <cassert>

#include <atomic>
#include <vector>
#include <string>
#include <string_view>
#include <variant>

namespace neujson {

enum Type {
  NEU_NULL, NEU_BOOL, NEU_INT32, NEU_INT64, NEU_DOUBLE, NEU_STRING, NEU_ARRAY, NEU_OBJECT
};

struct Member;

class Value;

using Data = std::variant<
    bool,
    int32_t,
    int64_t,
    double,
    ::std::shared_ptr<::std::vector<char>>,
    ::std::shared_ptr<::std::vector<Value>>,
    ::std::shared_ptr<::std::vector<Member>>
>;

class Value {
 public:
  using MemberIterator = ::std::vector<Member>::iterator;
  using ConstMemberIterator = ::std::vector<Member>::const_iterator;

 private:
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
  explicit Value(::std::string_view _s) : type_(NEU_STRING), data_(std::make_shared<String>(_s.begin(), _s.end())) {};
  explicit Value(const char *_s) : type_(NEU_STRING), data_(std::make_shared<String>(_s, _s + strlen(_s))) {};
  Value(const char *_s, ::std::size_t _len) : Value(::std::string_view(_s, _len)) {};
  Value(const Value &_val);
  Value(Value &&_val) noexcept;
  ~Value();

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
    return {&*s_ptr->begin(), s_ptr->size()};
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
};

struct Member {
  Member(Value &&_key, Value &&_value) : key_(std::move(_key)), value_(std::move(_value)) {}
  Member(std::string_view _key, Value &&_value) : key_(_key), value_(std::move(_value)) {}

  Value key_;
  Value value_;
};

} // namespace neujson

#endif //NEUJSON_NEUJSON_VALUE_H_
