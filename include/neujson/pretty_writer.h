//
// Created by HominSu on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_PRETTY_WRITER_H_
#define NEUJSON_NEUJSON_PRETTY_WRITER_H_

#include "neujson/neujson.h"
#include "neujson/writer.h"

#include <cstring>

#include <memory>

namespace neujson {

enum PrettyFormatOptions {
  kFormatDefault = 0,         //!< Default pretty formatting.
  kFormatSingleLineArray = 1  //!< Format arrays on a single line.
};

template<typename WriteStream>
class PrettyWriter : public Writer<WriteStream> {
 public:
  using Base = Writer<WriteStream>;

 protected:
  PrettyFormatOptions format_options_;

 private:
  ::std::string indent_;
  ::std::string_view indent_sv_;

 public:
  explicit PrettyWriter(WriteStream &_os) : Base(_os), format_options_(kFormatDefault) {
    InitIndent(' ', 2);
  }

  PrettyWriter &SetIndent(char _indent_char, ::std::size_t _indent_char_count) {
    NEUJSON_ASSERT(_indent_char == ' ' || _indent_char == '\t' || _indent_char == '\n' || _indent_char == '\r');
    InitIndent(_indent_char, _indent_char_count);
    return *this;
  }

  PrettyWriter &SetFormatOptions(PrettyFormatOptions _format_options) {
    format_options_ = _format_options;
    return *this;
  }

 public:
  //@formatter:off
  bool Null() { PrettyPrefix(NEU_NULL); return Base::EndValue(Base::WriteNull()); }
  bool Bool(bool _b) { PrettyPrefix(NEU_BOOL); return Base::EndValue(Base::WriteBool(_b)); }
  bool Int32(int32_t _i32) { PrettyPrefix(NEU_INT32); return Base::EndValue(Base::WriteInt32(_i32)); }
  bool Int64(int64_t _i64) { PrettyPrefix(NEU_INT64); return Base::EndValue(Base::WriteInt64(_i64)); }
  bool Double(double _d) { PrettyPrefix(NEU_DOUBLE); return Base::EndValue(Base::WriteDouble(_d)); }
  bool String(::std::string_view _str) { PrettyPrefix(NEU_STRING); return Base::EndValue(Base::WriteString(_str)); }
  bool Key(::std::string_view _str) { PrettyPrefix(NEU_STRING); return Base::EndValue(Base::WriteKey(_str)); }
  //@formatter:on

  bool StartObject() {
    PrettyPrefix(NEU_OBJECT);
    Base::stack_.emplace_back(false);
    return Base::EndValue(Base::WriteStartObject());
  }

  bool EndObject() {
    NEUJSON_ASSERT(!Base::stack_.empty());
    NEUJSON_ASSERT(!Base::stack_.back().in_array_flag_);
    NEUJSON_ASSERT(0 == Base::stack_.back().value_count_ % 2);
    bool empty = Base::stack_.back().value_count_ == 0;
    Base::stack_.pop_back();

    if (!empty) {
      Base::os_.put('\n');
      WriteIndent();
    }

    auto ret = Base::EndValue(Base::WriteEndObject());
    (void) ret;
    NEUJSON_ASSERT(ret == true);
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
    NEUJSON_ASSERT(!Base::stack_.empty());
    NEUJSON_ASSERT(Base::stack_.back().in_array_flag_);
    bool empty = Base::stack_.back().value_count_ == 0;
    Base::stack_.pop_back();

    if (!empty && !(format_options_ & kFormatSingleLineArray)) {
      Base::os_.put('\n');
      WriteIndent();
    }

    auto ret = Base::EndValue(Base::WriteEndArray());
    (void) ret;
    NEUJSON_ASSERT(ret == true);
    if (Base::stack_.empty()) {
      Base::Flush();
    }
    return true;
  }

 protected:
  void PrettyPrefix(Type type_);

 private:
  void InitIndent(char _indent_char, ::std::size_t _indent_char_count);
  void WriteIndent();
};

template<typename WriteStream>
void PrettyWriter<WriteStream>::PrettyPrefix(Type type_) {
  (void) type_;
  if (!Base::stack_.empty()) {
    auto &level = Base::stack_.back();

    if (level.in_array_flag_) {
      if (level.value_count_ > 0) {
        Base::os_.put(',');
        if (format_options_ & kFormatSingleLineArray) {
          Base::os_.put(' ');
        }
      }
      if (!(format_options_ & kFormatSingleLineArray)) {
        Base::os_.put('\n');
        WriteIndent();
      }
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
      NEUJSON_ASSERT(type_ == NEU_STRING && "miss quotation mark");
    }
    level.value_count_++;
  } else {
    NEUJSON_ASSERT(!Base::has_root_ && "root not singular");
    Base::has_root_ = true;
  }
}

template<typename WriteStream>
inline void PrettyWriter<WriteStream>::InitIndent(char _indent_char, size_t _indent_char_count) {
  indent_.clear();
  for (size_t i = 0; i < _indent_char_count; ++i) {
    indent_.push_back(_indent_char);
  }
  indent_sv_ = ::std::string_view(indent_);
}

template<typename WriteStream>
inline void PrettyWriter<WriteStream>::WriteIndent() {
  size_t count = Base::stack_.size();
  for (size_t i = 0; i < count; ++i) {
    Base::os_.put_sv(indent_sv_);
  }
}

} // namespace neujson

#endif //NEUJSON_NEUJSON_PRETTY_WRITER_H_
