#ifndef BASE_OBSERVER_LIST_THREADSAFE_H_
#define BASE_OBSERVER_LIST_THREADSAFE_H_

#include <algorithm>
#include <map>
#include <tuple>

#include "base/bind.h"

#include <glog/logging.h>
#include "base/macros.h"
#include "base/ref_counted.h"
#include "base/stl_util.h"
#include "base/observer_list.h"

#include <mutex>

#include <pthread.h>

#include <glog/logging.h>

namespace base {

// Forward declaration for ObserverListThreadSafeTraits.
template <class ObserverType>
class ObserverListThreadSafe;

namespace internal {

// An UnboundMethod is a wrapper for a method where the actual object is
// provided at Run dispatch time.
template <class T, class Method, class Params>
class UnboundMethod {
 public:
  UnboundMethod(Method m, const Params& p) : m_(m), p_(p) {
    static_assert((internal::ParamsUseScopedRefptrCorrectly<Params>::value),
                  "bad unbound method params");
  }
  void Run(T* obj) const {
    DispatchToMethod(obj, m_, p_);
  }
 private:
  Method m_;
  Params p_;
};

}  // namespace internal

template <class T>
struct ObserverListThreadSafeTraits {
  static void Destruct(const ObserverListThreadSafe<T>* x) {
    delete x;
  }
};

template <class ObserverType>
class ObserverListThreadSafe
    : public RefCountedThreadSafe<
        ObserverListThreadSafe<ObserverType>,
        ObserverListThreadSafeTraits<ObserverType>> {
 public:
  typedef typename ObserverList<ObserverType>::NotificationType NotificationType;

  ObserverListThreadSafe()
      : type_(ObserverListBase<ObserverType>::NOTIFY_ALL) {}

  explicit ObserverListThreadSafe(NotificationType type) : type_(type) {}

  void AddObserver(ObserverType* obs) {
    ObserverList<ObserverType>* list = nullptr;
    pthread_t thread_id = pthread_self();
    {
      std::lock_guard<std::mutex> lock(list_lock_);
      if (observer_lists_.find(thread_id) == observer_lists_.end()) {
        observer_lists_[thread_id] = new ObserverListContext(type_);
        list = &(observer_lists_[thread_id]->list);
        list->AddObserver(obs);
      }
    }
    //list->AddObserver(obs);
  }

  void RemoveObserver(ObserverType* obs) {
    ObserverListContext* context = nullptr;
    ObserverList<ObserverType>* list = nullptr;
    pthread_t thread_id = pthread_self();

    {
      std::lock_guard<std::mutex> lock(list_lock_);
      typename ObserversListMap::iterator it = observer_lists_.find(thread_id);
      if (it == observer_lists_.end()) {
        return;
      }
      context = it->second;
      list = &context->list;

      if (list->HasObserver(obs) && list->size() == 1) {
        observer_lists_.erase(it);
      }
    }
    list->RemoveObserver(obs);
    if (list->size() == 0) {
      delete context;
    }
  }

  void AssertEmpty() const {
    std::lock_guard<std::mutex> lock(list_lock_);
    DCHECK(observer_lists_.empty());
  }

  template<class Method, class... Params>
  void Notify(Method m, const Params&... params) {
    internal::UnboundMethod<ObserverType, Method, std::tuple<Params...>> method(
      m, std::make_tuple(params...));

    LOG(INFO) << "-----1";    
    std::lock_guard<std::mutex> lock(list_lock_);
    LOG(INFO) << "-----2";    
    for (const auto& entry : observer_lists_) {
      ObserverListContext* context = entry.second;
      LOG(INFO) << "-----3";    
      NotifyWrapper(context, method);
      LOG(INFO) << "-----4";    
    }
  }
  
 private:
  friend struct ObserverListThreadSafeTraits<ObserverType>;

  ~ObserverListThreadSafe() {
    STLDeleteValues(&observer_lists_);
  }

  struct ObserverListContext {
   public:
    explicit ObserverListContext(NotificationType type)
      : list(type) {} 

    ObserverList<ObserverType> list;

   private:
    DISALLOW_COPY_AND_ASSIGN(ObserverListContext);
  };

  template<class Method, class Params>
  void NotifyWrapper(ObserverListContext* context,
                     const internal::UnboundMethod<ObserverType, Method, Params>& method) {
    LOG(INFO) << "Inter NotifyWrapper";
    {
//      std::lock_guard<std::mutex> lock(list_lock_);
      typename ObserversListMap::iterator it = 
        observer_lists_.find(pthread_self());
      if (it == observer_lists_.end() || it->second != context) {
        return;
      }
    }

    LOG(INFO) << "------------a";

    {
      typename ObserverList<ObserverType>::Iterator it(&context->list);
      ObserverType* obs;
      while ((obs = it.GetNext()) != nullptr) {
        method.Run(obs);
      }
    }

    LOG(INFO) << "------------b";

    if (context->list.size() == 0) {
      {
//        std::lock_guard<std::mutex> lock(list_lock_);
        typename ObserversListMap::iterator it =
          observer_lists_.find(pthread_self());
        if (it != observer_lists_.end() && it->second == context) {
          observer_lists_.erase(it);
        }
      }
      delete context;
    }

    LOG(INFO) << "------------c";
  }


  typedef std::map<pthread_t, ObserverListContext*> ObserversListMap;

  mutable std::mutex list_lock_;
  ObserversListMap observer_lists_;
  const NotificationType type_;

  DISALLOW_COPY_AND_ASSIGN(ObserverListThreadSafe);
};

}  // namespace base

#endif  // BASE_OBSERVER_LIST_THREADSAFE_H_
