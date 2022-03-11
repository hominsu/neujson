//
// Created by HominSu on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_PRETTY_WRITER_H_
#define NEUJSON_NEUJSON_PRETTY_WRITER_H_

#include <neujson/writer.h>

#include <cassert>

namespace neujson {

template<typename WriteStream>
class PrettyWriter : public Writer<WriteStream> {
 public:
  using Base = Writer<WriteStream>;

 private:
  ::std::string_view indent_;
  int indent_depth_;
  bool expect_object_value_;

 public:
  explicit PrettyWriter(WriteStream &_os, ::std::string_view _indent = "  ")
      : Base(_os), indent_(_indent), indent_depth_(0), expect_object_value_(false) {}

 public:
  bool Null() {
    PrettyPrefix(NEU_NULL);
    return Base::EndValue(Base::WriteNull());
  }
  bool Bool(bool _b) {
    PrettyPrefix(NEU_BOOL);
    return Base::EndValue(Base::WriteBool(_b));
  }
  bool Int32(int32_t _i32) {
    PrettyPrefix(NEU_INT32);
    return Base::EndValue(Base::WriteInt32(_i32));
  }
  bool Int64(int64_t _i64) {
    PrettyPrefix(NEU_INT64);
    return Base::EndValue(Base::WriteInt64(_i64));
  }
  bool Double(double _d) {
    PrettyPrefix(NEU_DOUBLE);
    return Base::EndValue(Base::WriteDouble(_d));
  }
  bool String(::std::string_view _str) {
    PrettyPrefix(NEU_STRING);
    return Base::EndValue(Base::WriteString(_str));
  }
  bool Key(::std::string_view _str) {
    PrettyPrefix(NEU_STRING);
    return Base::EndValue(Base::WriteKey(_str));
  }

  bool StartObject() {
    PrettyPrefix(NEU_OBJECT);
    Base::stack_.emplace_back(false);
    return Base::EndValue(Base::WriteStartObject());
  }

  bool EndObject() {
    assert(!Base::stack_.empty());
    assert(!Base::stack_.back().in_array_flag_);
    assert(0 == Base::stack_.back().value_count_ % 2);
    bool empty = Base::stack_.back().value_count_ == 0;
    Base::stack_.pop_back();

    if (!empty) {
      Base::os_.put('\n');
      WriteIndent();
    }

    auto ret = Base::EndValue(Base::WriteEndObject());
    (void) ret;
    assert(ret == true);
    if (Base::stack_.empty()) {
      Base::Flush();
    }
    return true;
  }

  bool StartArray() {
    PrettyPrefix(NEU_ARRAY);
    Base::stack_.emplace_back(true);
    return Base::EndValue(Base::WriteStartArray());
  }

  bool EndArray() {
    assert(!Base::stack_.empty());
    assert(Base::stack_.back().in_array_flag_);
    bool empty = Base::stack_.back().value_count_ == 0;
    Base::stack_.pop_back();

    if (!empty) {
      Base::os_.put('\n');
      WriteIndent();
    }

    auto ret = Base::EndValue(Base::WriteEndArray());
    (void) ret;
    assert(ret == true);
    if (Base::stack_.empty()) {
      Base::Flush();
    }
    return true;
  }

 protected:
  void PrettyPrefix(Type type_);

 private:
  void WriteIndent() {
    size_t count = Base::stack_.size();
    for (size_t i = 0; i < count; ++i) {
      Base::os_.put(indent_);
    }
  }
};

template<typename WriteStream>
void PrettyWriter<WriteStream>::PrettyPrefix(Type type_) {
  if (!Base::stack_.empty()) {
    auto &level = Base::stack_.back();

    if (level.in_array_flag_) {
      if (level.value_count_ > 0) {
        Base::os_.put(',');
        Base::os_.put(' ');
      }
      Base::os_.put('\n');
      WriteIndent();
    } else {
      if (level.value_count_ > 0) {
        if (level.value_count_ % 2 == 0) {
          Base::os_.put(',');
          Base::os_.put('\n');
        } else {
          Base::os_.put(':');
          Base::os_.put(' ');
        }
      } else {
        Base::os_.put('\n');
      }

      if (level.value_count_ % 2 == 0) {
        WriteIndent();
      }
    }
    if (!level.in_array_flag_ && level.value_count_ % 2 == 0) {
      assert(type_ == NEU_STRING && "miss quotation mark");
    }
    level.value_count_++;
  } else {
    assert(!Base::has_root_ && "root not singular");
    Base::has_root_ = true;
  }
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_PRETTY_WRITER_H_
