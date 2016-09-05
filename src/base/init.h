#ifndef BASE_INIT_H_
#define BASE_INIT_H_

class MprInitializer {
 public:
  typedef void (*VoidFunction)(void);
  MprInitializer(const char* name, VoidFunction ctor, VoidFunction dtor)
      : /* name_(name), */ destructor_(dtor) {
    (void) name;
    if (ctor)
      ctor();
  }
  ~MprInitializer() {
    if (destructor_)
      destructor_();
  } 
    
 private:
  const VoidFunction destructor_;
};  
    
#define REGISTER_MODULE_INITIALIZER(name, body)                 \
  namespace {                                                   \
    static void mpr_init_module_##name () { body; }             \
      MprInitializer mpr_initializer_module_##name(#name,       \
               mpr_init_module_##name, NULL);                   \
}


#define REGISTER_MODULE_DESTRUCTOR(name, body)                  \
  namespace {                                                   \
       static void mpr_destruct_module_##name () { body; }      \
       MprInitializer google_destructor_module_##name(#name,    \
            NULL, mpr_destruct_module_##name);               \
}


#endif // BASE_INIT_H_
