constexpr int fibs[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

class True{};
class False{};

template<unsigned int N>
class Value {
    public:
    unsigned int a = N;
};

template<unsigned int N>
class Fibo {
    public:
    static constexpr unsigned int val = fibs[N];
};
    
template< typename Number >
class Fibin {
    public:
    
    template< typename T >
    static constexpr Number eval() {
        return T::val;
    }
};

#include <stdio.h>

int main() {
    printf("%d\n", Fibin<unsigned int>::eval< Fibo<0> >() );
    printf("%d\n", Fibin<unsigned int>::eval< Fibo<3> >() );
    printf("%d\n", Fibin<unsigned int>::eval< Fibo<5> >() );
    return 0;
}


/*
template<typename T> 
class Fibin{
    template<typename Expr, 
             typename = typename std::enable_if<std::is_integral<T>>::type, T result>
    constexpr T eval() {
        return 
    }
    
    void eval<
};*/
