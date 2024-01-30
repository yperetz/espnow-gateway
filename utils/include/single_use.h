#ifndef GATEWAY_UTILS_SINGLE_USE_H_
#define GATEWAY_UTILS_SINGLE_USE_H_

#include <assert.h>

namespace Gateway {

template <class T>
class SingleUse {
   protected:
    SingleUse() {
        static bool isFirstUse = true;
        assert(isFirstUse);
        isFirstUse = false;
    }

   private:
    SingleUse(const SingleUse<T> &);
    SingleUse &operator=(const SingleUse<T> &);
};

}  // namespace Gateway

#endif /* GATEWAY_UTILS_SINGLE_USE_H_ */