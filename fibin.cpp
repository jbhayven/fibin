#include <cstdint>

template<unsigned int N, typename T> 
struct Fib_eval {
    static constexpr T val = Fib_eval<N-1, T>::val + Fib_eval<N-2, T>::val;
};
template<typename T> 
struct Fib_eval<0, T > {
    static constexpr T val = 0;
};
template<typename T> 
struct Fib_eval<1, T > {
    static constexpr T val = 1;
};

////////////////////

struct True {};
struct False {};

template<unsigned int N> struct Fibo {};

template<typename T, typename... context > 
struct Lit { };

template<typename... context> 
struct Lit<True, context...> { 
    static constexpr bool val = true;
};

template<typename... context> 
struct Lit<False, context...> { 
    static constexpr bool val = false;
};

template<unsigned int U, typename... context, typename T> 
struct Lit< Fibo<U>, T, context... > {
    static constexpr auto val = Fib_eval<U, T>::val;
};

////////////////////

template<typename T, typename... context>
struct evaluate {};

template< typename... Targs, template<typename...> typename T, typename... context >
struct evaluate<T<Targs...>, context...> {
    static constexpr auto val = T<Targs..., context...>::val;
};

template< int i, typename... Targs, template<int, typename...> typename T, typename... context >
struct evaluate<T<i, Targs...>, context...> {
    static constexpr auto val = T<i, Targs..., context...>::val;
};

////////////////////

template<typename Condition, typename IfTrue, typename IfFalse, typename... context> 
struct If {
    static constexpr auto val = 
        evaluate <Condition, context...>::val ?
        evaluate <IfTrue, context...>::val :
        evaluate <IfFalse, context...>::val;
};

////////////////////

template<typename Left, typename Right, typename... context>
struct Eq {
    static constexpr bool val = 
        (evaluate<Left, context...>::val == evaluate<Right, context...>::val);
};

////////////////////

using VarID = uint32_t;

template<VarID id, typename Value> 
struct Variable { };

////////////////////

template <VarID Var, typename Value, typename Expression, typename... context> 
struct Let { 
    static constexpr bool val =
        evaluate<Expression, Variable<Var, Value>, context...>::val; 
};

////////////////////

#include <type_traits>

template<VarID Var, typename... context>
struct Lookup_evaluate { };

template<VarID Var, VarID id, typename Value, template<VarID, typename> typename Variable, typename... context> 
struct Lookup_evaluate<Var, Variable<id, Value>, context...> {
    static constexpr auto val = 
        Var == id ?
        evaluate<Value, context...>::val :
        Lookup_evaluate<Var, context...>::val;
};

////////////////////

template <VarID Var, typename... context>
struct Ref {
    static constexpr auto val = Lookup_evaluate<Var, context...>::val;
};

////////////////////

template <VarID Var, typename Body, typename... context>
struct Lambda { };

////////////////////

template <typename Fun, typename Param, typename... context>
struct Invoke { };

template < VarID id, typename Body, template <VarID, typename> typename Lambda, typename Param, typename... context>
struct Invoke<Lambda<id, Body>, Param, context...> { 
    static constexpr auto val = evaluate<Body, Variable<id, Param>, context...>::val;
};

////////////////////

template<typename... args> struct Sum{};


constexpr VarID Var(const char *name) {
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

#include <typeinfo>
#include <iostream>

template< typename Number >
class Fibin {
    public:
    
    static void eval() {
        std::cout << "Fibin doesn't support this type!\n";
    }
    
    template< typename T, typename = typename std::enable_if_t<std::is_integral<Number>::value > >
    static constexpr Number eval() {
        return evaluate<T, Number>::val;
    }
};
#include <stdio.h>

int main() {
    
    printf("%d\n", Fibin<unsigned int>::eval< Lit<Fibo<6> > > () );
    
    printf("%d\n", Fibin<unsigned int>::eval< 
    If< 
        If< 
            Lit<True>, 
            Lit<False>,
            Lit<True>
        >, 
        Lit<Fibo<10>>, 
        Lit<Fibo<2>> 
    > 
    >() );

    /*
    printf("%llu\n", Fibin<uint64_t>::eval<
        Invoke<
            Let<
                Var("x"), 
                Lit< Fibo<0> >, 
                Lambda<
                    Var("x"), 
                    Ref<Var("x")>
                >
            >, 
            Lit< Fibo<1> >
            >
        >());
        */
    /*
    printf("%llu\n", Fibin<uint64_t>::eval<
        Let<
            Var("f"),
            Lambda<
                Var("x"),
                Ref<Var("x")>
            >,
            Invoke<Ref<Var("f")>, Lit<Fibo<0> > >
        >
    >());
    */
    
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
