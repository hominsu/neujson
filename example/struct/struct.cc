//
// Created by Homin Su on 2022/3/15.
//

#include "neujson/document.h"
#include "neujson/file_write_stream.h"
#include "neujson/pretty_writer.h"
#include "neujson/value.h"

#include <cassert>
#include <cstdio>

struct SerialInfo {
  ::std::string serial_port_;
  int32_t baud_rate_;
  int32_t data_bits_;
  int32_t stop_bits_;
  int32_t parity_;
  bool flow_control_;
  bool clocal_;

  explicit SerialInfo(const neujson::Value &_val) {
    switch (_val.GetType()) {
      case neujson::NEU_OBJECT:serial_port_ = _val["serial_port"].GetString();
        baud_rate_ = _val["baud_rate"].GetInt32();
        data_bits_ = _val["data_bits"].GetInt32();
        stop_bits_ = _val["stop_bits"].GetInt32();
        parity_ = _val["parity"].GetInt32();
        flow_control_ = _val["flow_control"].GetBool();
        clocal_ = _val["clocal"].GetBool();
        break;
      default:assert(false && "bad type");
    }
  }

  [[nodiscard]] neujson::Value toJsonObject() const {
    neujson::Value value(neujson::NEU_OBJECT);
    value.AddMember("serial_port", serial_port_);
    value.AddMember("baud_rate", baud_rate_);
    value.AddMember("data_bits", data_bits_);
    value.AddMember("stop_bits", stop_bits_);
    value.AddMember("parity", parity_);
    value.AddMember("flow_control", flow_control_);
    value.AddMember("clocal", clocal_);
    return value;
  }
};

int main() {
  neujson::Value value(neujson::NEU_OBJECT);
  value.AddMember("serial_port", "/dev/cu.usbserial-AB0JHQVJ");
  value.AddMember("baud_rate", 115200);
  value.AddMember("data_bits", 8);
  value.AddMember("stop_bits", 1);
  value.AddMember("parity", 0);
  value.AddMember("flow_control", false);
  value.AddMember("clocal", true);

  SerialInfo serial_info(value);

  value = serial_info.toJsonObject();
  char writeBuffer[65536];
  neujson::FileWriteStream out(stdout, writeBuffer, sizeof(writeBuffer));
  neujson::Writer<neujson::FileWriteStream> writer(out);
  value.WriteTo(writer);

  return 0;
}
