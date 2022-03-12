//
// Created by Homin Su on 2022/3/9.
//

#include "document.h"

neujson::Value *neujson::Document::addValue(Value &&_value) {
  Type type = _value.getType();
  (void) type;
  if (see_value_) { assert(!stack_.empty() && "root not singular"); }
  else {
    assert(type_ == NEU_NULL);
    see_value_ = true;
    type_ = _value.type_;

    switch (type) {
      case NEU_NULL: break;
      case NEU_BOOL:data_ = ::std::get<bool>(_value.data_);
        break;
      case NEU_INT32:data_ = ::std::get<int32_t>(_value.data_);
        break;
      case NEU_INT64:data_ = ::std::get<int64_t>(_value.data_);
        break;
      case NEU_DOUBLE:data_ = ::std::get<double>(_value.data_);
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
    top.value->addValue(::std::move(_value));
    top.valueCount++;
    return top.lastValue();
  } else {
    assert(top.type() == NEU_OBJECT);
    if (top.valueCount % 2 == 0) {
      assert(type == NEU_STRING && "miss quotation mark");
      key_ = ::std::move(_value);
      top.valueCount++;
      return &key_;
    } else {
      top.value->addMember(::std::move(key_), ::std::move(_value));
      top.valueCount++;
      return top.lastValue();
    }
  }
}
