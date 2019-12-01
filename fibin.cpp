template<unsigned int N> struct Fibo {
    enum { val = Fibo<N-1>::val + Fibo<N-2>::val };
};
template<> struct Fibo<0> {
    enum { val = 0 };
};
template<> struct Fibo<1> {
    enum { val = 1 };
};

////////////////////
class True {};
class False {};

template<typename T> struct Lit { };

template<> struct Lit<True> { 
    enum { logical = 1 };
};

template<> struct Lit<False> {
    enum { logical = 0 };
};

template<unsigned int U> struct Lit< Fibo<U> > {
    enum { val = Fibo<U>::val };
};
////////////////////

constexpr int Var(const char *name) {
    int returned = 0;
    
    int i = 0; 
    while(i < 6 && name[i] != 0) {
        if(!(
            ('0' <= name[i] && name[i] <= '9') || 
            ('a' <= name[i] && name[i] <= 'z') ||
            ('A' <= name[i] && name[i] <= 'Z')
            )
        )
        returned = -1;
        i++;
    }

    if(i < 1 || i > 6) returned = -1;
    
    if(returned == 0){
        int result = 0;
        int factor = 1;
        
        while(i--) {
            if('0' <= name[i] && name[i] <= '9') 
                result += (name[i] - '0') * factor;
            else if('A' <= name[i] && name[i] <= 'Z')
                result += (10 + name[i] - 'A') * factor;
            else
                result += (10 + name[i] - 'a') * factor;          
            
            factor *= (('z' - 'a' + 1) + ('9' - '0' + 1));
        }

        returned = result;
    }
    
    return returned;
}

template<typename... args> struct Sum{};

template<int i, typename T, typename U> struct Let {};

template<int i, typename T, typename... args>
struct Let<i, T, Sum<args...> > {
    enum { val = Sum<args..., Fibo<i>, T>::val }; 
};

template<typename T> struct Invoke { };


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
    printf("%d\n", Fibin<unsigned int>::eval< Fibo<Var("a")> >() );
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
