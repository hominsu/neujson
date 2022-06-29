//
// Created by Homin Su on 2022/3/13.
//

#ifndef NEUJSON_NEUJSON_NEUJSON_H_
#define NEUJSON_NEUJSON_NEUJSON_H_

#if defined(__has_builtin)
#define NEUJSON_HAS_BUILTIN(x) __has_builtin(x)
#else
#define NEUJSON_HAS_BUILTIN(x) 0
#endif

#ifndef NEUJSON_ASSERT
#include <cassert>
#define NEUJSON_ASSERT(x) assert(x)
#endif // NEUJSON_ASSERT

/*
 * @brief Avoid compiler warnings
 */
#ifndef NEUJSON_UINT64_C2
#define NEUJSON_UINT64_C2(high32, low32) ((static_cast<uint64_t>(high32) << 32) | static_cast<uint64_t>(low32))
#endif

#endif //NEUJSON_NEUJSON_NEUJSON_H_
