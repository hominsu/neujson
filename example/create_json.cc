//
// Created by Homing So on 2022/3/15.
//

#include <cstdio>

#include "neujson/file_write_stream.h"
#include "neujson/writer.h"

int main() {
  char writeBuffer[65536];
  neujson::FileWriteStream os(stdout, writeBuffer);
  neujson::Writer writer(os);

  writer.StartArray();
  writer.Double(std::numeric_limits<double>::infinity());
  writer.Double(std::numeric_limits<double>::quiet_NaN());
  writer.Bool(true);
  writer.String(R"({"happy": true, "pi": 3.141})");
  writer.Bool(true);
  writer.EndArray();

  return 0;
}
