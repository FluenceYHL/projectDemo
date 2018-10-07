#ifndef ANY_H
#define ANY_H

#include <iostream>
#include <functional>
#include <type_traits>
#include <mutex>

namespace YHL {

    class any {
    private:
        // any 内部通过一个基类指针, 控制模板派生类
        class base_holder {
        public:
            virtual ~base_holder();
        public:
            virtual const std::type_info& type() const noexcept = 0;
            virtual base_holder* clone() const = 0;
        };

        // 将 any 泛化的类型延迟到派生类的模板参数 T 中
        // any 只需要含有一个基类指针便可以操控多种对象了
        template<typename T>
        // 如果是私有继承, 外面访问不到derived 的 base 部分
        class derived_holder : public base_holder {
        public:
            T content;
        public:
            derived_holder(const T& rhs)
                : content(rhs)
            {}
            virtual const std::type_info& type() const noexcept {
                return typeid(T);
            }
            virtual base_holder* clone() const {
                return new derived_holder(content);
            }
        private:
            // 允许拷贝构造, 但是禁止赋值, 因为赋值会失去原来的 content
            derived_holder& operator=(const derived_holder&) = delete;
        };

    private:
        base_holder *ptr;
        std::mutex mtx;    // 适当加入线程控制

    public:
// ------------------ 构造函数部分 ---------------------
        any() noexcept : ptr(nullptr) {}

        ~any() noexcept {
            if(ptr not_eq nullptr) {
                std::lock_guard<std::mutex> lck(mtx);
                delete ptr;
                ptr = nullptr;
            }
        }
        // any 拷贝构造
        any(const any& rhs)
            : ptr(rhs.ptr == nullptr ? nullptr : rhs.ptr->clone())
        {}

        // 根据内部引用的构造函数
        template<typename T>
        any(const T& value)
            : ptr(new derived_holder<typename std::remove_cv<typename std::decay<const T>::type>::type>(value))
        {}

// ------------------ 修改部分 ------------------------

        any& swap(any& rhs) noexcept {

            std::lock_guard<std::mutex> lck(mtx);

            // 交换内部引用, 提高效率, pimpl 手法; swap 异常安全
            std::swap(this->ptr, rhs.ptr);
            return *this;
        }

        any& operator=(const any& rhs) {
            any(rhs).swap(*this);   // 临时对象会被销毁
            return *this;
        }

        any& operator=(any&& rhs) {
            rhs.swap(*this);
            any().swap(rhs);       // rhs 在转移后为空
            return *this;
        }

        // 以上是针对 any 的赋值, 接下来是针对 T
        template<typename T>
        any& operator=(const T &rhs) {
            any(rhs).swap(*this);
            return *this;
        }

        // 完美语义转发
        template<typename T>
        any& operator=(T&& rhs) {
            any(static_cast<T&&>(rhs)).swap(*this);
            return *this;
        }

// ------------------ 查询部分 ------------------------
        bool empty() const noexcept {
            return ptr == nullptr;
        }

        void clear() noexcept {
            any().swap(*this);
        }

        const std::type_info& type() const noexcept {
            return ptr ? ptr->type() : typeid(void);
        }

    private:

        template<typename T>
        friend T* any_cast(any*) noexcept;

        template<typename T>
        friend T* unsafe_any_cast(any*) noexcept;
    };

    // 两个操作数时, 提供 non-member function, 覆盖 std::swap
    inline void swap(any& lhs, any&rhs) {
        lhs.swap(rhs);
    }

// ------------------- any_cast 相关 -------------------
    class bad_any_cast : std::bad_cast {
    public:
        virtual const char* what() noexcept;
    private:
        using std::bad_cast::what; // 和基类虚函数定义不一致，但同名，需要使用 using 声明, 也可 private
    };


    // 如果类型相同, 就返回 derived_holder 内部引用的地址
    template<typename T>
    T* any_cast(any *operand) noexcept {
        using realType = typename std::remove_cv<T>::type;
        return operand and operand->type() == typeid(T) ?
            std::addressof(static_cast<any::derived_holder<realType> *>(operand->ptr)->content)
            //static_cast<any::derived_holder<realType> >(operand->ptr)->content)
            : nullptr;
    }

    // 这里我不明白, 返回值的  const T* 是怎么做的 ?
    template<typename T>
    inline const T* any_cast(const any *operand) noexcept {
        return any_cast<T>(const_cast<any*>(operand));
    }

    // 根据以上的两种针对 T 指针的返回值, * 得到最终的 T
    template<typename T>
    T any_cast(any &operand) {
        typedef typename std::remove_reference<T>::type non_ref;

        non_ref* res = any_cast<non_ref>(std::addressof(operand));

        if(res == nullptr)
            throw bad_any_cast();

        typedef typename std::conditional<
            std::is_reference<T>::value,
            T,
            typename std::add_lvalue_reference<T>::type  // 如何加上右值引用呢 ?
        >::type with_ref;

        return static_cast<with_ref>(*res);
    }

    template <typename T>
    inline T any_cast(const any &operand) {

        typedef typename std::remove_reference<T>::type non_ref;

        return any_cast<const non_ref&>(const_cast<any&>(operand));
    }

    template <typename T>
    inline T* unsafe_any_cast(any *operand) noexcept {
        return std::addressof(
            static_cast<any::derived_holder<T> >(operand->ptr)->content
        );
    }

    template <typename T>
    inline T* unsafe_any_cast(const any* operand) noexcept {
        return unsafe_any_cast<T>(const_cast<any*>(operand));
    }

}

#endif // ANY_H
