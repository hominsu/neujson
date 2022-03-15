//
// Created by Homin Su on 2022/3/9.
//

#include "document.h"

neujson::Value *neujson::Document::addValue(Value &&_value) {
  Type type = _value.GetType();
  (void) type;
  if (see_value_) { NEUJSON_ASSERT(!stack_.empty() && "root not singular"); }
  else {
    NEUJSON_ASSERT(type_ == NEU_NULL);
    see_value_ = true;
    type_ = _value.type_;

    switch (type) {
      case NEU_NULL: break;
      case NEU_BOOL:data_ = ::std::get<NEU_BOOL_TYPE>(_value.data_);
        break;
      case NEU_INT32:data_ = ::std::get<NEU_INT32_TYPE>(_value.data_);
        break;
      case NEU_INT64:data_ = ::std::get<NEU_INT64_TYPE>(_value.data_);
        break;
      case NEU_DOUBLE:data_ = ::std::get<NEU_DOUBLE_TYPE>(_value.data_);
        break;
      case NEU_STRING:data_ = ::std::get<NEU_STRING_TYPE>(_value.data_);
        ::std::get<NEU_STRING_TYPE>(_value.data_) = nullptr;
        break;
      case NEU_ARRAY:data_ = ::std::get<NEU_ARRAY_TYPE>(_value.data_);
        ::std::get<NEU_ARRAY_TYPE>(_value.data_) = nullptr;
        break;
      case NEU_OBJECT:data_ = ::std::get<NEU_OBJECT_TYPE>(_value.data_);
        ::std::get<NEU_OBJECT_TYPE>(_value.data_) = nullptr;
        break;
    }
    _value.type_ = NEU_NULL;
    return this;
  }

  auto &top = stack_.back();
  if (top.type() == NEU_ARRAY) {
    top.value->AddValue(::std::move(_value));
    top.value_count++;
    return top.lastValue();
  } else {
    NEUJSON_ASSERT(top.type() == NEU_OBJECT);
    if (top.value_count % 2 == 0) {
      NEUJSON_ASSERT(type == NEU_STRING && "miss quotation mark");
      key_ = ::std::move(_value);
      top.value_count++;
      return &key_;
    } else {
      top.value->AddMember(::std::move(key_), ::std::move(_value));
      top.value_count++;
      return top.lastValue();
    }
  }
}
