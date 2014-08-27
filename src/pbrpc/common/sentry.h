#ifndef _PBRPC_COMMON_SENTRY_H__
#define _PBRPC_COMMON_SENTRY_H__

namespace pbrpc {
namespace common {

template <class T>
class DefaultDeleter {
};

template <class T>
class DefaultDeleter < T* > {
public:
    void operator()(T* val) {
        delete val;
    }
};

/// @brief �Զ��ͷ���Դ���ڱ�����������std::auto_ptr���ǲ��ܿ��������Ӱ�ȫ���׳���
template <class T, T DefaultValue = T(), class Deleter = DefaultDeleter<T>>
class Sentry {
public:
    explicit Sentry() : val_(DefaultValue) {}
    explicit Sentry(const Deleter& deleter) : val_(DefaultValue), deleter_(deleter) {}
    explicit Sentry(const T& obj) : val_(obj) {}
    explicit Sentry(const T& obj, const Deleter& deleter) : val_(obj), deleter_(deleter) {}
    ~Sentry() {
        Clear();
    }
    const T& Get() const {
        return val_;
    }
    /// @note only valid for pointers
    T& operator -> () {
        return val_;
    }
    /// @note only valid for pointers
    const T& operator -> () const {
        return val_;
    }
    void Reset(const T& newVal) {
        Clear();
        val_ = newVal;
    }
    T Release() {
        T tmp = val_;
        val_ = DefaultValue;
        return tmp;
    }
    /// @brief ֱ�ӷ����ڲ����������
    /// @note only use it when you exactly knonw what you are doing!!!
    T& Raw() {
        return val_;
    }
private:
    Sentry(const Sentry&);
    Sentry& operator= (const Sentry&);
    bool operator == (const Sentry&) const;
    void Clear() {
        if (val_ != DefaultValue) {
            deleter_(val_);
            val_ = DefaultValue;
        }
    }
private:
    T           val_;
    Deleter     deleter_;
};

}
}

#endif // _PBRPC_COMMON_SENTRY_H__