//
// Created by Homin Su on 2022/3/7.
//

#ifndef NEUJSON_NEUJSON_NONCOPYABLE_H_
#define NEUJSON_NEUJSON_NONCOPYABLE_H_

namespace neujson {

class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;

 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

} // namespace neujson

#endif //NEUJSON_NEUJSON_NONCOPYABLE_H_
