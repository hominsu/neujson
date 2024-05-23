//
// Created by Homing So on 2022/3/11.
//

#ifndef NEUJSON_NEUJSON_PRETTY_WRITER_H_
#define NEUJSON_NEUJSON_PRETTY_WRITER_H_

#include <cstdint>
#include <cstring>

#include <memory>

#include "internal/ieee754.h"
#include "neujson.h"
#include "writer.h"

namespace neujson {

enum PrettyFormatOptions {
  kFormatDefault = 0,        //!< Default pretty formatting.
  kFormatSingleLineArray = 1 //!< Format arrays on a single line.
};

template <typename WriteStream>
class PrettyWriter : public Writer<WriteStream> {
public:
  using Base = Writer<WriteStream>;

protected:
  PrettyFormatOptions format_options_;

private:
  std::string indent_;
  std::string_view indent_sv_;

public:
  explicit PrettyWriter(WriteStream &os)
      : Base(os), format_options_(kFormatDefault) {
    InitIndent(' ', 4);
  }

  PrettyWriter &SetIndent(const char indent_char,
                          const std::size_t indent_char_count) {
    NEUJSON_ASSERT(indent_char == ' ' || indent_char == '\t' ||
                   indent_char == '\n' || indent_char == '\r');
    InitIndent(indent_char, indent_char_count);
    return *this;
  }

  PrettyWriter &SetFormatOptions(const PrettyFormatOptions format_options) {
    format_options_ = format_options;
    return *this;
  }

  bool Null() override {
    PrettyPrefix(NEU_NULL);
    return Base::EndValue(Base::WriteNull());
  }

  bool Bool(bool b) override {
    PrettyPrefix(NEU_BOOL);
    return Base::EndValue(Base::WriteBool(b));
  }

  bool Int32(int32_t i32) override {
    PrettyPrefix(NEU_INT32);
    return Base::EndValue(Base::WriteInt32(i32));
  }

  bool Int64(int64_t i64) override {
    PrettyPrefix(NEU_INT64);
    return Base::EndValue(Base::WriteInt64(i64));
  }

  bool Double(double d) override {
    PrettyPrefix(NEU_DOUBLE);
    return Base::EndValue(Base::WriteDouble(internal::Double(d)));
  }

  bool Double(internal::Double d) override {
    PrettyPrefix(NEU_DOUBLE);
    return Base::EndValue(Base::WriteDouble(d));
  }

  bool String(std::string_view str) override {
    PrettyPrefix(NEU_STRING);
    return Base::EndValue(Base::WriteString(str));
  }

  bool Key(std::string_view str) override {
    PrettyPrefix(NEU_STRING);
    return Base::EndValue(Base::WriteKey(str));
  }

  bool StartObject() override {
    PrettyPrefix(NEU_OBJECT);
    Base::stack_.emplace_back(false);
    return Base::EndValue(Base::WriteStartObject());
  }

  bool EndObject() override {
    NEUJSON_ASSERT(!Base::stack_.empty());
    NEUJSON_ASSERT(!Base::stack_.back().in_array_flag_);
    NEUJSON_ASSERT(0 == Base::stack_.back().value_count_ % 2);
    const bool empty = Base::stack_.back().value_count_ == 0;
    Base::stack_.pop_back();

    if (!empty) {
      Base::os_.put('\n');
      WriteIndent();
    }

    auto ret = Base::EndValue(Base::WriteEndObject());
    (void)ret;
    NEUJSON_ASSERT(ret == true);
    if (Base::stack_.empty()) {
      Base::Flush();
    }
    return true;
  }

  bool StartArray() override {
    PrettyPrefix(NEU_ARRAY);
    Base::stack_.emplace_back(true);
    return Base::EndValue(Base::WriteStartArray());
  }

  bool EndArray() override {
    NEUJSON_ASSERT(!Base::stack_.empty());
    NEUJSON_ASSERT(Base::stack_.back().in_array_flag_);
    bool empty = Base::stack_.back().value_count_ == 0;
    Base::stack_.pop_back();

    if (!empty && !(format_options_ & kFormatSingleLineArray)) {
      Base::os_.put('\n');
      WriteIndent();
    }

    auto ret = Base::EndValue(Base::WriteEndArray());
    (void)ret;
    NEUJSON_ASSERT(ret == true);
    if (Base::stack_.empty()) {
      Base::Flush();
    }
    return true;
  }

protected:
  void PrettyPrefix(Type type);

private:
  void InitIndent(char indent_char, std::size_t indent_char_count);
  void WriteIndent();
};

template <typename WriteStream>
void PrettyWriter<WriteStream>::PrettyPrefix(const Type type) {
  (void)type;
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
      NEUJSON_ASSERT(type == NEU_STRING && "miss quotation mark");
    }
    ++level.value_count_;
  } else {
    NEUJSON_ASSERT(!Base::has_root_ && "root not singular");
    Base::has_root_ = true;
  }
}

template <typename WriteStream>
void PrettyWriter<WriteStream>::InitIndent(
    const char indent_char, const std::size_t indent_char_count) {
  indent_.clear();
  for (std::size_t i = 0; i < indent_char_count; ++i) {
    indent_.push_back(indent_char);
  }
  indent_sv_ = std::string_view(indent_);
}

template <typename WriteStream> void PrettyWriter<WriteStream>::WriteIndent() {
  const std::size_t count = Base::stack_.size();
  for (std::size_t i = 0; i < count; ++i) {
    Base::os_.put_sv(indent_sv_);
  }
}

} // namespace neujson

#endif // NEUJSON_NEUJSON_PRETTY_WRITER_H_
