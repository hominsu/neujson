//
// Created by Homin Su on 2022/3/5.
//

#include <neujson/value.h>

neujson::Value::~Value() = default;

neujson::Value::Value(neujson::Type _type) : type_(_type), data_() {
  switch (type_) {
    case NEU_NULL:
    case NEU_BOOL:
    case NEU_INT32:
    case NEU_INT64:
    case NEU_DOUBLE:break;
    case NEU_STRING:data_ = std::make_shared<String>();
      break;
    case NEU_ARRAY:data_ = std::make_shared<Array>();
      break;
    case NEU_OBJECT:data_ = std::make_shared<Object>();
      break;
    default: assert(false && "bad value getType");
  }
}

neujson::Value::Value(const neujson::Value &_val) = default;

neujson::Value::Value(neujson::Value &&_val) noexcept: type_(_val.type_), data_(::std::move(_val.data_)) {}

::std::size_t neujson::Value::getSize() {
  switch (type_) {
    case NEU_ARRAY:return ::std::get<StringWithSharedPtr>(data_)->size();
    case NEU_OBJECT:return ::std::get<ObjectWithSharedPtr>(data_)->size();
    default: return 1;
  }
}

neujson::Value &neujson::Value::operator=(const neujson::Value &_val) {
  assert(this != &_val);
  this->~Value();
  type_ = _val.type_;
  data_ = _val.data_;
  return *this;
}

neujson::Value &neujson::Value::operator=(neujson::Value &&_val) noexcept {
  assert(this != &_val);
  this->~Value();
  type_ = _val.type_;
  data_ = std::move(_val.data_);
  _val.type_ = NEU_NULL;
  return *this;
}
