

#if !defined(__klib_lazy_h__)
#define __klib_lazy_h__

#include <functional>
#include <stdexcept>

namespace klib {
namespace pattern {

template<typename T>
class lazy 
{
private:
    typedef lazy<T>         self_type;

    // init function type
    typedef   std::function<void(T&t)>  init_functor;

    static void default_initiator(T&t) {
        throw std::runtime_error("No lazy evaluator given.");
    }

    init_functor initiator_;
    T    value_;
    bool initialized_;

public:
    lazy() : initiator_(default_initiator), initialized_(false) { }
    lazy(init_functor initiator) : initiator_(initiator), initialized_(false) { }
    lazy(const lazy<T>& other) : initiator_(other.initiator_), initialized_(false) { }

    lazy<T>& operator= (const lazy<T>& other) {
        initiator_   = other.initiator_;
        initialized_ = false;
        return *this;
    }

    self_type& set_lazy_initor(const init_functor& func) {
        initiator_ = func;
        return *this;
    }

    T& get_value() {
        if (!initialized_) {
            initiator_(value_);
            initialized_ = true;
        }
        return value_;
    }

    T& operator* () {
        return get_value();
    }
};

template<typename T>
class lazy_safe 
{
private:
    static T default_initiator() {
        throw std::runtime_error("No lazy evaluator given.");
    }

    std::function<T()> initiator_;
    std::mutex lock_;
    T    value_;
    bool initialized_;

public:
    lazy_safe() : initiator_(default_initiator) { }
    lazy_safe(std::function<T()> initiator) : initiator_(initiator), initialized_(false) { }
    lazy_safe(const lazy_safe<T>& other) : initiator_(other.initiator_), initialized_(false) { }

    lazy_safe<T>& operator= (const lazy_safe<T>& other) {
        lock_.lock();
        initiator_   = other.initiator_;
        initialized_ = false;
        lock_.unlock();
        return *this;
    }

    T& get_value() {
        if (!initialized_) {
            lock_.lock();
            if (!initialized_) {
                value_       = initiator_();
                initialized_ = true;
            }
            lock_.unlock();
        }
        return value_;
    }

    T& operator* () {
        return get_value();
    }
};

}}


#endif  /* __klib_lazy_h__ */
