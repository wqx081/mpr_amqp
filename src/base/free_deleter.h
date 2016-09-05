#ifndef BASE_FREE_DELETER_H_
#define BASE_FREE_DELETER_H_
#include <stdlib.h>

namespace base {

struct FreeDeleter {
  inline void operator()(void* ptr) const {
    free(ptr);
  }
};

} // namespace base
#endif // BASE_FREE_DELETER_H_
