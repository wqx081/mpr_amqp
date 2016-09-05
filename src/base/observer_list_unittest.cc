#include "base/observer_list.h"
#include "base/observer_list_threadsafe.h"

#include <vector>


#include "base/weak_ptr.h"

#include <gtest/gtest.h>
#include <glog/logging.h>

namespace base {

namespace {

class Foo {
 public:
  virtual void Observe(int x) = 0;
  virtual ~Foo() {}
};

class Adder : public Foo {
 public:
  explicit Adder(int scaler) : total(0), scaler_(scaler) {}
  void Observe(int x) override {
    total += x * scaler_;
  }
  ~Adder() override {}
  int total;

 private:
  int scaler_;
};

class Disrupter : public Foo {
 public:
  Disrupter(ObserverList<Foo>* list, Foo* doomed)
      : list_(list),
        doomed_(doomed) {}
  ~Disrupter() override {}
  void Observe(int x) override { (void)x; list_->RemoveObserver(doomed_); }

 private:
  ObserverList<Foo>* list_;
  Foo* doomed_;
};

class ThreadSafeDisrupter : public Foo {
 public:
  ThreadSafeDisrupter(ObserverListThreadSafe<Foo>* list, Foo* doomed)
        : list_(list),
    doomed_(doomed) {
  }
  ~ThreadSafeDisrupter() override {}
  void Observe(int x) override { (void) x; list_->RemoveObserver(doomed_); }
  
 private:
  ObserverListThreadSafe<Foo>* list_;
  Foo* doomed_;

};

template <typename ObserverListType>
class AddInObserve : public Foo {
 public:
  explicit AddInObserve(ObserverListType* observer_list)
        : added(false),
    observer_list(observer_list),
    adder(1) {
  }
  
  void Observe(int x) override {
    (void) x;
    if (!added) {
      added = true;
      observer_list->AddObserver(&adder);
    }
  }
  
  bool added;
  ObserverListType* observer_list;
  Adder adder;
};


} // namespace 

TEST(ObserverList, Basic) {
    ObserverList<Foo> observer_list;
    Adder a(1), b(-1), c(1), d(-1), e(-1);
    Disrupter evil(&observer_list, &c);
  
    observer_list.AddObserver(&a);
    observer_list.AddObserver(&b);
  
    EXPECT_TRUE(observer_list.HasObserver(&a));
    EXPECT_FALSE(observer_list.HasObserver(&c));
  
    FOR_EACH_OBSERVER(Foo, observer_list, Observe(10));
  
    observer_list.AddObserver(&evil);
    observer_list.AddObserver(&c);
    observer_list.AddObserver(&d);
  
    observer_list.RemoveObserver(&e);
  
    FOR_EACH_OBSERVER(Foo, observer_list, Observe(10));
  
    EXPECT_EQ(20, a.total);
    EXPECT_EQ(-20, b.total);
    EXPECT_EQ(0, c.total);
    EXPECT_EQ(-10, d.total);
    EXPECT_EQ(0, e.total);
}

TEST(ObserverListThreadSafeTest, BasicTest) {
  
    scoped_ref_ptr<ObserverListThreadSafe<Foo> > observer_list(
        new ObserverListThreadSafe<Foo>);
    Adder a(1);
    Adder b(-1);
    Adder c(1);
    Adder d(-1);
    ThreadSafeDisrupter evil(observer_list.get(), &c);
  
    observer_list->AddObserver(&a);
    observer_list->AddObserver(&b);
  
    observer_list->Notify(&Foo::Observe, 10);

    observer_list->AddObserver(&evil);
    observer_list->AddObserver(&c);
    observer_list->AddObserver(&d);
  
    observer_list->Notify(&Foo::Observe, 10);
  
    EXPECT_EQ(20, a.total);
    EXPECT_EQ(-20, b.total);
    EXPECT_EQ(0, c.total);
    EXPECT_EQ(-10, d.total);
}

} // namespace base
