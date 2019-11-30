constexpr int fibs[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

class True{};
class False{};

template<unsigned int N>
class Value {
    public:
    unsigned int a = N;
};

template<unsigned int N>
struct Fibo {
    enum{ val = fibs[N] };
};

template< typename T >
unsigned int eval() {
    return T::val;
}

#include <stdio.h>

int main() {
    printf("%d\n", eval< Fibo<0> >() );
    printf("%d\n", eval< Fibo<2> >() );
    printf("%d\n", eval< Fibo<5> >() );
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
