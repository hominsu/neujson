//
// Created by Homing So on 2022/3/5.
//

#ifndef NEUJSON_NEUJSON_VALUE_H_
#define NEUJSON_NEUJSON_VALUE_H_

#include <cstdint>
#include <cstring>

#include <algorithm>
#include <atomic>
#include <memory>
#include <ranges>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "internal/ieee754.h"
#include "neujson.h"

namespace neujson {

namespace required::handler {
namespace details {

template <typename Handler>
concept HasNull = requires(Handler handler) {
  { handler.Null() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasBool = requires(Handler handler, bool b) {
  { handler.Bool(b) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasInt32 = requires(Handler handler, int32_t i32) {
  { handler.Int32(i32) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasInt64 = requires(Handler handler, int64_t i64) {
  { handler.Int64(i64) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasDouble = requires(Handler handler, internal::Double d) {
  { handler.Double(d) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasString = requires(Handler handler, std::string_view sv) {
  { handler.String(sv) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasKey = requires(Handler handler, std::string_view sv) {
  { handler.Key(sv) } -> std::same_as<bool>;
};

template <typename Handler>
concept HasStartObject = requires(Handler handler) {
  { handler.StartObject() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasEndObject = requires(Handler handler) {
  { handler.EndObject() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasStartArray = requires(Handler handler) {
  { handler.StartArray() } -> std::same_as<bool>;
};

template <typename Handler>
concept HasEndArray = requires(Handler handler) {
  { handler.EndArray() } -> std::same_as<bool>;
};

} // namespace details

template <typename T>
concept HasAllRequiredFunctions =
    details::HasNull<T> && details::HasBool<T> && details::HasInt32<T> &&
    details::HasInt64<T> && details::HasDouble<T> && details::HasString<T> &&
    details::HasKey<T> && details::HasStartObject<T> &&
    details::HasEndObject<T> && details::HasStartArray<T> &&
    details::HasEndArray<T>;

} // namespace required::handler

#undef SUFFIX
#define VALUE(NEU)                                                             \
  NEU(NULL, std::monostate) SUFFIX NEU(BOOL, bool) SUFFIX NEU(INT32, int32_t)  \
  SUFFIX NEU(INT64, int64_t)                                                   \
  SUFFIX NEU(DOUBLE, internal::Double)                                         \
      SUFFIX NEU(STRING, std::shared_ptr<std::vector<char>>)                   \
          SUFFIX NEU(ARRAY, std::shared_ptr<std::vector<Value>>)               \
              SUFFIX NEU(OBJECT, std::shared_ptr<std::vector<Member>>) //

enum Type {
#define VALUE_NAME(_name, _type) NEU_##_name
#define SUFFIX ,
  VALUE(VALUE_NAME)
#undef SUFFIX
#undef VALUE_NAME
};

class Value;
struct Member;

using Data = std::variant<
#define VALUE_TYPE(_name, _type) _type
#define SUFFIX ,
    VALUE(VALUE_TYPE)
#undef SUFFIX
#undef VALUE_TYPE
    >;

class Document;

class Value {
public:
  using MemberIterator = std::vector<Member>::iterator;
  using ConstMemberIterator = std::vector<Member>::const_iterator;

#define VALUE_TYPE(_name, _type) using NEU_##_name##_TYPE = _type;
#define SUFFIX
  VALUE(VALUE_TYPE)
#undef SUFFIX
#undef VALUE_TYPE

private:
  friend class Document;

  using String = std::vector<char>;
  using Array = std::vector<Value>;
  using Object = std::vector<Member>;

  Type type_;
  Data data_;

public:
  explicit Value(Type type = NEU_NULL);
  explicit Value(bool b) : type_(NEU_BOOL), data_(b) {};
  explicit Value(int32_t i32) : type_(NEU_INT32), data_(i32) {};
  explicit Value(int64_t i64) : type_(NEU_INT64), data_(i64) {};
  explicit Value(double d) : type_(NEU_DOUBLE), data_(internal::Double(d)) {};
  explicit Value(internal::Double d) : type_(NEU_DOUBLE), data_(d) {};
  explicit Value(const char *s)
      : type_(NEU_STRING), data_(std::make_shared<String>(s, s + strlen(s))) {};
  explicit Value(std::string_view s)
      : type_(NEU_STRING),
        data_(std::make_shared<String>(s.begin(), s.end())) {};
  Value(const char *s, const std::size_t len)
      : Value(std::string_view(s, len)) {};
  Value(const Value &val) = default;
  Value(Value &&val) noexcept
      : type_(val.type_), data_(std::move(val.data_)) {};
  ~Value() = default;

  [[nodiscard]] Type GetType() const { return type_; };
  [[nodiscard]] std::size_t GetSize() const;

  [[nodiscard]] bool IsNull() const { return type_ == NEU_NULL; }
  [[nodiscard]] bool IsBool() const { return type_ == NEU_BOOL; }
  [[nodiscard]] bool IsInt32() const { return type_ == NEU_INT32; }
  [[nodiscard]] bool IsInt64() const {
    return type_ == NEU_INT64 || type_ == NEU_INT32;
  }
  [[nodiscard]] bool IsDouble() const { return type_ == NEU_DOUBLE; }
  [[nodiscard]] bool IsString() const { return type_ == NEU_STRING; }
  [[nodiscard]] bool IsArray() const { return type_ == NEU_ARRAY; }
  [[nodiscard]] bool IsObject() const { return type_ == NEU_OBJECT; }

  // getter
  //@formatter:off
  [[nodiscard]] bool GetBool() const {
    NEUJSON_ASSERT(type_ == NEU_BOOL);
    return std::get<NEU_BOOL_TYPE>(data_);
  }
  [[nodiscard]] int32_t GetInt32() const {
    NEUJSON_ASSERT(type_ == NEU_INT32);
    return std::get<NEU_INT32_TYPE>(data_);
  }

  [[nodiscard]] int64_t GetInt64() const {
    NEUJSON_ASSERT(type_ == NEU_INT64 || type_ == NEU_INT32);
    return type_ == NEU_INT64 ? std::get<NEU_INT64_TYPE>(data_)
                              : std::get<NEU_INT32_TYPE>(data_);
  }

  [[nodiscard]] double GetDouble() const {
    NEUJSON_ASSERT(type_ == NEU_DOUBLE);
    return std::get<NEU_DOUBLE_TYPE>(data_).Value();
  }

  [[nodiscard]] std::string_view GetStringView() const {
    NEUJSON_ASSERT(type_ == NEU_STRING);
    auto s_ptr = std::get<NEU_STRING_TYPE>(data_);
    return {s_ptr->data(), s_ptr->size()};
  }

  [[nodiscard]] std::string GetString() const {
    return std::string(GetStringView());
  }
  [[nodiscard]] const auto &GetArray() const {
    NEUJSON_ASSERT(type_ == NEU_ARRAY);
    return std::get<NEU_ARRAY_TYPE>(data_);
  }
  [[nodiscard]] const auto &GetObject() const {
    NEUJSON_ASSERT(type_ == NEU_OBJECT);
    return std::get<NEU_OBJECT_TYPE>(data_);
  }
  //@formatter:on

  // setter
  Value &SetNull() {
    this->~Value();
    return *new (this) Value(NEU_NULL);
  }

  Value &SetBool(bool _b) {
    this->~Value();
    return *new (this) Value(_b);
  }

  Value &SetInt32(int32_t _i32) {
    this->~Value();
    return *new (this) Value(_i32);
  }

  Value &SetInt64(int64_t _i64) {
    this->~Value();
    return *new (this) Value(_i64);
  }

  Value &SetDouble(double _d) {
    this->~Value();
    return *new (this) Value(_d);
  }

  Value &SetString(std::string_view _s) {
    this->~Value();
    return *new (this) Value(_s);
  }

  Value &SetArray() {
    this->~Value();
    return *new (this) Value(NEU_ARRAY);
  }

  Value &SetObject() {
    this->~Value();
    return *new (this) Value(NEU_OBJECT);
  }

  Value &operator=(const Value &val);
  Value &operator=(Value &&val) noexcept;

  Value &operator[](size_t index);
  const Value &operator[](size_t index) const;
  Value &operator[](std::string_view key);
  const Value &operator[](std::string_view key) const;

  template <typename T> Value &AddValue(T &&value);

  MemberIterator MemberBegin();
  MemberIterator MemberEnd();
  MemberIterator FindMember(std::string_view key);

  [[nodiscard]] ConstMemberIterator MemberBegin() const;
  [[nodiscard]] ConstMemberIterator MemberEnd() const;
  [[nodiscard]] ConstMemberIterator FindMember(std::string_view key) const;

  template <typename T> Value &AddMember(const char *key, T &&value);
  Value &AddMember(Value &&key, Value &&value);

  template <required::handler::HasAllRequiredFunctions Handler>
  bool WriteTo(Handler &handler) const;
};

#undef VALUE

struct Member {
  Member(Value &&key, Value &&value)
      : key_(std::move(key)), value_(std::move(value)) {}
  Member(std::string_view key, Value &&value)
      : key_(key), value_(std::move(value)) {}

  Value key_;
  Value value_;
};

inline Value::Value(Type type) : type_(type) {
  switch (type_) {
  case NEU_NULL:
  case NEU_BOOL:
  case NEU_INT32:
  case NEU_INT64:
  case NEU_DOUBLE:
    break;
  case NEU_STRING:
    data_ = std::make_shared<String>();
    break;
  case NEU_ARRAY:
    data_ = std::make_shared<Array>();
    break;
  case NEU_OBJECT:
    data_ = std::make_shared<Object>();
    break;
  default:
    NEUJSON_ASSERT(false && "bad value GetType");
  }
}

inline std::size_t Value::GetSize() const {
  switch (type_) {
  case NEU_ARRAY:
    return std::get<NEU_STRING_TYPE>(data_)->size();
  case NEU_OBJECT:
    return std::get<NEU_OBJECT_TYPE>(data_)->size();
  default:
    return 1;
  }
}

inline Value &Value::operator=(const Value &val) {
  NEUJSON_ASSERT(this != &val);
  type_ = val.type_;
  data_ = val.data_;
  return *this;
}

inline Value &Value::operator=(Value &&val) noexcept {
  NEUJSON_ASSERT(this != &val);
  type_ = val.type_;
  data_ = std::move(val.data_);
  val.type_ = NEU_NULL;
  return *this;
}

inline Value &Value::operator[](const std::size_t index) {
  NEUJSON_ASSERT(type_ == NEU_ARRAY);
  return std::get<NEU_ARRAY_TYPE>(data_)->at(index);
}

inline const Value &Value::operator[](const std::size_t index) const {
  NEUJSON_ASSERT(type_ == NEU_ARRAY);
  return std::get<NEU_ARRAY_TYPE>(data_)->at(index);
}

inline Value &Value::operator[](const std::string_view key) {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  auto it = FindMember(key);
  if (it != std::get<NEU_OBJECT_TYPE>(data_)->end()) {
    return it->value_;
  }

  NEUJSON_ASSERT(false && "value no found");
  static Value fake(NEU_NULL);
  return fake;
}

inline const Value &Value::operator[](const std::string_view key) const {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this)[key];
}

template <typename T> Value &Value::AddValue(T &&value) {
  NEUJSON_ASSERT(type_ == NEU_ARRAY);
  auto a_ptr = std::get<NEU_ARRAY_TYPE>(data_);
  a_ptr->emplace_back(std::forward<T>(value));
  return a_ptr->back();
}

inline Value::MemberIterator Value::MemberBegin() {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return std::get<NEU_OBJECT_TYPE>(data_)->begin();
}

inline Value::MemberIterator Value::MemberEnd() {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return std::get<NEU_OBJECT_TYPE>(data_)->end();
}

inline Value::MemberIterator Value::FindMember(const std::string_view key) {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return std::ranges::find_if(*std::get<NEU_OBJECT_TYPE>(data_),
                              [key](const Member &_m) -> bool {
                                return _m.key_.GetStringView() == key;
                              });
}

inline Value::ConstMemberIterator Value::MemberBegin() const {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this).MemberBegin();
}

inline Value::ConstMemberIterator Value::MemberEnd() const {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this).MemberEnd();
}

inline Value::ConstMemberIterator
Value::FindMember(const std::string_view key) const {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  return const_cast<Value &>(*this).FindMember(key);
}

template <typename T> Value &Value::AddMember(const char *key, T &&value) {
  return AddMember(Value(key), Value(std::forward<T>(value)));
}

inline Value &Value::AddMember(Value &&key, Value &&value) {
  NEUJSON_ASSERT(type_ == NEU_OBJECT);
  NEUJSON_ASSERT(key.type_ == NEU_STRING);
  NEUJSON_ASSERT(FindMember(key.GetStringView()) == MemberEnd());
  auto o_ptr = std::get<NEU_OBJECT_TYPE>(data_);
  o_ptr->emplace_back(std::move(key), std::move(value));
  return o_ptr->back().value_;
}

#define CALL_HANDLER(_expr)                                                    \
  do {                                                                         \
    if (!(_expr)) {                                                            \
      return false;                                                            \
    }                                                                          \
  } while (false)

template <required::handler::HasAllRequiredFunctions Handler>
bool Value::WriteTo(Handler &handler) const {
  switch (type_) {
  case NEU_NULL:
    CALL_HANDLER(handler.Null());
    break;
  case NEU_BOOL:
    CALL_HANDLER(handler.Bool(std::get<NEU_BOOL_TYPE>(data_)));
    break;
  case NEU_INT32:
    CALL_HANDLER(handler.Int32(std::get<NEU_INT32_TYPE>(data_)));
    break;
  case NEU_INT64:
    CALL_HANDLER(handler.Int64(std::get<NEU_INT64_TYPE>(data_)));
    break;
  case NEU_DOUBLE:
    CALL_HANDLER(handler.Double(std::get<NEU_DOUBLE_TYPE>(data_)));
    break;
  case NEU_STRING:
    CALL_HANDLER(handler.String(GetStringView()));
    break;
  case NEU_ARRAY:
    CALL_HANDLER(handler.StartArray());
    for (auto &val : *GetArray()) {
      CALL_HANDLER(val.WriteTo(handler));
    }
    CALL_HANDLER(handler.EndArray());
    break;
  case NEU_OBJECT:
    CALL_HANDLER(handler.StartObject());
    for (auto &mem : *GetObject()) {
      handler.Key(mem.key_.GetStringView());
      CALL_HANDLER(mem.value_.WriteTo(handler));
    }
    CALL_HANDLER(handler.EndObject());
    break;
  default:
    NEUJSON_ASSERT(false && "bad type");
  }
  return true;
}

#undef CALL_HANDLER

} // namespace neujson

#endif // NEUJSON_NEUJSON_VALUE_H_
