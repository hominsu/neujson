#include <neujson/value.h>

#include <cstdio>

#define ADD_MEMBER(_obj, _key, _value) \
  do { \
    (_obj).addMember((_key), (_value)); \
  } while (false) \

#define TEST_VALUE(_obj, _key, _value, _getter, _format) \
  do { \
    ADD_MEMBER((_obj), (_key), (_value));\
    printf(#_key ": " _format "\n", _obj[_key]._getter); \
  } while (false) \

int main() {

  neujson::Value value(neujson::NEU_OBJECT);

  TEST_VALUE(value, "string", "this is a string", getString().c_str(), "%s");
  TEST_VALUE(value, "bool", true, getBool(), "%d");
  TEST_VALUE(value, "i64", INT64_MAX, getInt64(), "%lld");

  return 0;
}
