//
// Created by Homing So on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_NON_COPYABLE_H_
#define NEUJSON_NEUJSON_NON_COPYABLE_H_

namespace neujson {

class NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;

protected:
  NonCopyable() = default;
  ~NonCopyable() = default;
};

} // namespace neujson

#endif // NEUJSON_NEUJSON_NON_COPYABLE_H_
