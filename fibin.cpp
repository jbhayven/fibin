constexpr int fibs[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};

class True{};
class False{};

template<unsigned int N>
class Value {
    public:
    unsigned int a = N;
};

template<int N>
class Fibo {
    public:
    unsigned int a = fibs[N];
};

template< template<unsigned int U> Fibo<U> f  >
unsigned int eval() {
    return f;
}

#include <stdio.h>

int main() {
    printf("%d\n", eval< Fibo<0> >() );
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
