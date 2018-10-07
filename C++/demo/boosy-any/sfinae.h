#ifndef SFINAE_H
#define SFINAE_H

namespace SFINAE {
    // 定义判断类型
    typedef char yes ;
    typedef struct{ char str[2] ; } no ;

    /* 判断 T 是否存在迭代器
     * yes judge 函数负责判断成功的条件
     * no judge 中 ... 接收除了 yes judge 以外的所有条件
     * 尝试所有的 judge 匹配后, 判断得到的 judge 数据类型的大小
     */
    template<typename T>
    struct has_iterator {
        template<typename other>
        static yes judge(typename other::iterator* x) ;
        template<typename other>
        static no judge(...) ;
        const static bool value = sizeof(judge<T>(0)) == sizeof(yes) ;
    } ;

    /* 判断 T 是不是一个 class
    * yes judge 函数负责判断成功的条件
    * no judge 中 ... 接收除了 yes judge 以外的所有条件
    * 尝试所有的 judge 匹配后, 判断得到的 judge 数据类型的大小
    * 判断 T 是不是一个 class
    */
    template<typename T>
    struct is_class {
        template<typename other>
        static yes judge(int other::*) ;
        template<typename other>
        static no judge(...) ;
        const static bool value = sizeof(judge<T>(nullptr)) == sizeof(yes) ;
    } ;

    // 判断 T 是不是一个指针
    template<typename T>
    struct is_pointer {
        static T object ;
        template<typename other>
        static yes judge(other*) ;
        // template<typename other>
        static no judge(...) ;
        const static bool value = sizeof(judge(object)) == sizeof(yes) ;
    } ;

    // 判断 T 是否有这个函数 function
    template<typename T, typename returnValue, typename ...Args>
    struct has_function {
        template<typename other, returnValue (other::*)(Args...)>
        struct help ;
        template<typename other>
        static yes judge(help<other, &other::function> *) ;
        template<typename other>
        static no judge(...) ;
        const static bool value = sizeof(judge<T>(0)) == sizeof(yes) ;
    } ;
}

// 类型萃取, 更改参数类型
namespace SFINAE {
    // 选择实际传入的参数类型
    template<typename T>
    struct parameterOption {
        // 原始
        typedef T parameter ;   // 实际传入的参数
        typedef T origin ;      // 保存原始参数类型
        // 引用
        typedef T& ref ;        // 引用
        typedef T& refOrigin ;  // 引用的原始参数类型
        // const
        typedef T const constOne ;
        typedef T const& constRef ;
    } ;

    // 针对 const 的局部特化
    template<typename T>
    struct parameterOption<T const> {
        // 原始
        typedef T const parameter ;
        typedef T origin ;
        // 引用
        typedef T const& ref ;
        typedef T& refOrigin ;
        // const
        typedef T const constOne ;
        typedef T const& constRef ;
    } ;

    // 针对 & 引用的局部特化
    template<typename T>
    struct parameterOption<T &> {
        // 原始
        typedef T &parameter ;
        typedef typename parameterOption<T>::origin origin ;
        // 引用
        typedef T &ref ;
        typedef typename parameterOption<T>::origin &refOrigin ;
        // const
        typedef T const constOne ;
        typedef T const& constRef ;
    } ;

    // 针对 void 的全特化
    // 如果演绎得到 void, 得到的始终是 void, 避免出现无效类型
    template<>
    struct parameterOption<void> {
        typedef void parameter ;
        typedef void origin ;
        typedef void ref ; // void const
        typedef void refOrigin ;
        typedef void constOne ;
        typedef void constRef ;
    } ;
}

// 从多个类型中选择, 类似于 conditional, enable_if
namespace SFINAE {
    template<bool C, typename A, typename B>
    struct enable_if ;
    // true 局部特化
    template<typename A, typename B>
    struct enable_if<true, A, B> { typedef A result ; } ;
    // false 局部特化
    template<typename A, typename B>
    struct enable_if<false, A, B> { typedef B result ; } ;
}

// 类型提升
namespace SFINAE {
    // 基本模板
    template<typename A, typename B>
    struct promotion {
        typedef typename enable_if<( sizeof(A) > sizeof(B) ), A,
            typename enable_if<( sizeof(A) < sizeof(B) ), B, void>::result>::result result ;
    } ;

    // 针对两个相同的 T 的局部特化
    template<typename T>
    struct promotion<T, T> { typedef T result ; } ;

    // 提升为更高的类型
    #define MORE_PROMOTION(A, B, C) \
    template<>                       \
    struct promotion<A, B> { typedef C result ; } ; \

    /* 其他类型提升, 类似容器元素
    template<typename A, typename B>
    struct promotion< std:vector<A>, std::vector<B> > {
        typedef std::vector<typename promotion<A, B>::result> result ;
    } ;
    // promotion<A, B> 和 promotion<vector<A>, vector<B>> 特化程度是一样的
    // 会产生二义性, 所以需要更特化一个版本
    template<typename T>
    struct promotion< std:vector<T>, std::vector<T> > {
        typedef std::vector<typename promotion<T, T>::result> result ;
    } ;*/
}

// 根据是否为 class 类传引用或者传值
namespace SFINAE {
    // 针对非 const 类型传 const&
    template<typename T>
    struct passOption {
        typedef typename enable_if<is_class<T>::value, const T&, T>::result result ;
    } ;
    // 针对 const 类型传 &引用
    template<typename T>
    struct passOption<T const> {
        typedef typename enable_if<is_class<T>::value, T&, T>::result result ;
    } ;
    // 针对内置类型等不需要拷贝构造, 传值比传引用效率高
    template<>
    struct passOption<double> { typedef long result ; } ;
}

// 测试函数
namespace SFINAE {
    template<typename A, typename B>
    void foo_core(typename passOption<A>::result lchild,
                  typename passOption<B>::result rchild) {

    }

    template<typename A, typename B>
    inline void foo(A const& lchild, B const& rchild) {
        foo_core<A, B>(lchild, rchild) ;
    }

    // no match function for call to 'function(double&, main()::example&'
    // 不能直接传参数 double 给 typename, 因为 double 对应的是 A const& 中的 A, 而不是 result
    template<typename A, typename B>
    inline void function(typename passOption<A const&>::result lchild,
                         typename passOption<B const&>::result rchild) {

    }
}

#endif // SFINAE_H
