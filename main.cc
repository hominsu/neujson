#if defined(_WINDOWS) && defined(Debug)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <neujson/document.h>
#include <neujson/writer.h>
#include <neujson/file_write_stream.h>
#include "example/sample.h"

#include <cstdio>

#include <iostream>

#define ADD_MEMBER(_obj, _key, _value) \
  do { \
    (_obj).addMember((_key), (_value)); \
  } while (false) \

#define TEST_VALUE(_obj, _key, _value, _getter, _format) \
  do { \
    ADD_MEMBER((_obj), (_key), (_value));\
    fprintf(stdout, #_key ": " _format "\n", _obj[_key]._getter); \
  } while (false) \

int main() {
#if defined(_WINDOWS) && defined(Debug)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
  {
    neujson::Value value(neujson::NEU_OBJECT);

    TEST_VALUE(value, "string", "this is a string", getString().c_str(), "%s");
    TEST_VALUE(value, "bool", true, getBool(), "%d");
    TEST_VALUE(value, "i64", INT64_MAX, getInt64(), "%lld");

    try {
      throw neujson::Exception(neujson::error::PARSE_BAD_STRING_CHAR);
    } catch (::std::exception &_e) {
      fprintf(stdout, "%s\n", _e.what());
    }
  }

  {
    neujson::Document document;
    neujson::error::ParseError err = document.parse(
        R"({
          "precision": "zip",
          "Latitude": 37.766800000000003,
          "Longitude": -122.3959,
          "Address": "asd",
          "City": "SAN FRANCISCO",
          "State": "CA",
          "Zip": "94107",
          "Country": "US"
        })"
    );

    if (err != neujson::error::PARSE_OK) {
      puts(neujson::parseErrorStr(err));
      exit(1);
    }

    // get 'precision' field
    neujson::Value &precision = document["precision"];
    std::cout << "precision: " << precision.getString() << '\n';

    // get 'Latitude' field
    neujson::Value &latitude = document["Latitude"];
    std::cout << "Latitude: " << latitude.getDouble() << "\n";

    // get 'Longitude' field
    neujson::Value &longitude = document["Longitude"];
    std::cout << "Longitude: " << longitude.getDouble() << "\n";

    // get 'Address' field
    neujson::Value &address = document["Address"];
    std::cout << "Address: " << address.getString() << '\n';

    // get 'City' field
    neujson::Value &city = document["City"];
    std::cout << "City: " << city.getString() << '\n';

    // get 'State' field
    neujson::Value &state = document["State"];
    std::cout << "State: " << state.getString() << '\n';

    // get 'Zip' field
    neujson::Value &zip = document["Zip"];
    std::cout << "Zip: " << zip.getString() << "\n";

    // get 'Country' field
    neujson::Value &country = document["Country"];
    std::cout << "Country: " << country.getString() << "\n";

    // set 'Zip' field
    zip.setInt32(9527);
    std::cout << "Zip: " << zip.getInt32() << "\n";
  }

  {
    for (auto &sv: sample) {
      fprintf(stdout, "\n");

      neujson::Document document;
      neujson::error::ParseError err = document.parse(sv);
      if (err != neujson::error::PARSE_OK) {
        puts(neujson::parseErrorStr(err));
        return 1;
      }

      neujson::FileWriteStream os(stdout);
      neujson::Writer writer(os);
      document.writeTo(writer);

      fprintf(stdout, "\n");
    }
  }

  return 0;
}
