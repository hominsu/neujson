//
// Created by Homin Su on 2022/3/5.
//
#if defined(_WINDOWS) && defined(Debug)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "value.h"

#include <algorithm>

neujson::Value::Value(neujson::Type _type) : type_(_type), data_() {
  switch (type_) {
    case NEU_NULL:
    case NEU_BOOL:
    case NEU_INT32:
    case NEU_INT64:
    case NEU_DOUBLE:break;
    case NEU_STRING:data_ = ::std::make_shared<String>();
      break;
    case NEU_ARRAY:data_ = ::std::make_shared<Array>();
      break;
    case NEU_OBJECT:data_ = ::std::make_shared<Object>();
      break;
    default: assert(false && "bad value getType");
  }
}

::std::size_t neujson::Value::getSize() {
  switch (type_) {
    case NEU_ARRAY:return ::std::get<StringWithSharedPtr>(data_)->size();
    case NEU_OBJECT:return ::std::get<ObjectWithSharedPtr>(data_)->size();
    default: return 1;
  }
}

neujson::Value::MemberIterator neujson::Value::findMember(::std::string_view _key) {
  assert(type_ == NEU_OBJECT);
  return ::std::find_if(::std::get<ObjectWithSharedPtr>(data_)->begin(),
                        ::std::get<ObjectWithSharedPtr>(data_)->end(),
                        [_key](const Member &_m) -> bool {
                          return _m.key_.getStringView() == _key;
                        });
}
