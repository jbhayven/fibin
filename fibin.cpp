template<unsigned int N> struct Fibo {
    enum { val = Fibo<N-1>::val + Fibo<N-2>::val };
};
template<> struct Fibo<0> {
    enum { val = 0 };
};
template<> struct Fibo<1> {
    enum { val = 1 };
};

class True {};
class False {};

template<typename T> struct Lit { Lit<T>() = delete; };

template<> struct Lit<True> { 
    enum { logical = 1 };
};

template<> struct Lit<False> {
    enum { logical = 0 };
};

template<unsigned int U> struct Lit< Fibo<U> > {
    enum { val = Fibo<U>::val };
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
    
    printf("%d\n", Fibin<unsigned int>::eval< Lit<Fibo<6> > > () );
    
    //printf("%d\n", Fibin<unsigned int>::eval< Lit<int> > () );
    
    //printf("%d\n", Fibin<unsigned int>::eval< Lit<True> >() );
    
    //Lit<int> a;
    
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
