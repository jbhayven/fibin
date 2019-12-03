#include <cstdint>

#define NO_VARIABLE 0
using VarID = uint32_t;

template<VarID id, typename Value> 
struct Variable { };

////////////////////

template<typename T, T _value>
struct Value{ 
    static constexpr T value = _value;
};

////////////////////

template<typename... context> struct Get_numeric;

template<typename T, typename... context> 
struct Get_numeric<T, context...> {
    using type = typename Get_numeric<context...>::type;
};

template<typename T>
struct Get_numeric<T> {
    using type = T;
};

////////////////////

template<VarID id, typename T, typename... context>
struct Function { };

template<typename T, typename... context> 
struct Function<NO_VARIABLE, T, context...>{
    static constexpr typename Get_numeric<context...>::type val = T::value;
};

////////////////////

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

template<unsigned int N> struct Fib {};

template<typename T, typename... context > 
struct Lit { };

template<typename... context> 
struct Lit<True, context...> { 
    using fun = 
        Function<
            NO_VARIABLE,
            Value<bool, true>,
            context...
        >;
};

template<typename... context> 
struct Lit<False, context...> { 
    using fun = 
        Function<
            NO_VARIABLE,
            Value<bool, false>,
            context...
        >;
};

template<unsigned int U, typename... context> 
struct Lit< Fib<U>, context... > {
    using fun = 
        Function<
            NO_VARIABLE,
            Value<
                typename Get_numeric<context...>::type,
                Fib_eval<U, typename Get_numeric<context...>::type>::val
            >,
            context...
        >;
};

////////////////////

template<typename Expression, typename... context> struct expr_evaluate;

template<typename... Args, template<typename...> typename Expression, typename... context >
struct expr_evaluate<Expression<Args...>, context...> {
    using fun = typename Expression<Args..., context...>::fun;
};

template< VarID id, typename... Args, template<VarID, typename...> typename Expression, typename... context >
struct expr_evaluate<Expression<id, Args...>, context...> {
    using fun = typename Expression<id, Args..., context...>::fun;
};

////////////////////

template<typename T> struct evaluate;

template<VarID id, typename Body, typename... context>
struct evaluate<Function<id, Body, context...>> {
    using fun = typename expr_evaluate <Body, context...>::fun;
};

////////////////////

#include <type_traits>

template<typename Condition, typename IfTrue, typename IfFalse, typename... context> 
struct If {
    using fun = typename
        std::conditional <
            expr_evaluate <Condition, context...>::fun::val,
            typename expr_evaluate <IfTrue, context...>::fun,
            typename expr_evaluate <IfFalse, context...>::fun
        >::type;
};

////////////////////

template<typename T, typename New>
struct enrich { };

template<VarID id, typename Body, typename... context, typename Value>
struct enrich <Function<id, Body, context...>, Value> {
    using fun = Function<id, Body, Variable<id, Value>, context...>;
};

template<VarID id, typename Body, typename... context, VarID newID, typename newValue>
struct enrich<Function<id, Body, context...>,  Variable<newID, newValue>> {
   using fun = Function<id, Body, Variable<newID, newValue>, context...>;
};

///////////////////

template<typename Left, typename Right, typename... context>
struct Eq {
    using fun =
        Function<
            NO_VARIABLE, 
            Value<
                bool,
                (expr_evaluate<Left, context...>::fun::val == expr_evaluate<Right, context...>::fun::val)
            >,
            context...
        >;
};

////////////////////

template <VarID id, typename Value, typename T, typename... context> 
struct Let {
    using fun = 
        typename expr_evaluate<
            T, 
            Variable<
                id, 
                expr_evaluate<Value, context...>
            >, context...
        >::fun;
};

////////////////////

template <VarID Var, typename... context> struct Lookup;

template <VarID Var, typename T, typename... context>
struct Lookup<Var, T, context...> {
    using fun = typename Lookup<Var, context...>::fun;
};

template <VarID Var, typename T, typename... context>
struct Lookup<Var, Variable<Var, T>, context...> {
    using fun = typename T::fun;
};

////////////////////

template <VarID Var, typename... context>
struct Ref {
    using fun = typename Lookup<Var, context...>::fun;
};

////////////////////

template <VarID Var, typename Body, typename... context>
struct Lambda { 
    using fun = Function<Var, Body, context...>;
};

////////////////////

template <typename Fun, typename Param, typename... context>
struct Invoke {
    using fun =
        typename evaluate< 
            typename enrich< 
                typename expr_evaluate<Fun, context...>::fun, 
                expr_evaluate<Param, context...>
            >::fun
        >::fun;
};

////////////////////

template<typename Arg, typename... context> 
struct Inc1 {
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                typename Get_numeric<context...>::type, 
                expr_evaluate<Arg, context...>::fun::val + 1
            >,
            context...
        >;
};

template<typename Arg, typename... context> 
struct Inc10 {
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                typename Get_numeric<context...>::type, 
                expr_evaluate<Arg, context...>::fun::val + 10
            >,
            context...
        >;
};

////////////////////

template<typename... Args> struct Sum;

template<typename T, typename... Args>
struct Sum<T, Args...> {
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                typename Get_numeric<Args...>::type, 
                expr_evaluate<T, Args...>::fun::val + Sum<Args...>::fun::val
            >,
            Args...
        >;
};

template<typename T, typename Number>
struct Sum<T, Number> {
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                Number,
                expr_evaluate<T, Number>::fun::val
            >,
            Number
        >;
};

template<typename T, VarID id, typename Val, typename... Args>
struct Sum<T, Variable<id, Val>, Args...> {
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                typename Get_numeric<Args...>::type, 
                expr_evaluate<T, Variable<id, Val>, Args...>::fun::val
            >,
            Args...
        >;
};


////////////////////

constexpr VarID Var(const char *name) {
    VarID returned = NO_VARIABLE + 1;
    
    int i = 0; 
    while(i < 6 && name[i] != 0) {
        if(!(
            ('0' <= name[i] && name[i] <= '9') || 
            ('a' <= name[i] && name[i] <= 'z') ||
            ('A' <= name[i] && name[i] <= 'Z')
            )
        ) returned = NO_VARIABLE;
        
        i++;
    }

    if(i < 1 || i > 6) returned = NO_VARIABLE;
    
    if(returned != NO_VARIABLE){
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
    
    template< typename T, typename U = Number, typename = typename std::enable_if_t<!std::is_integral<U>::value > >
    static void eval() {
        std::cout << "Fibin doesn't support " << typeid(Number).name() << "!\n";
    }
    
    template< typename T, typename U = Number, typename = typename std::enable_if_t<std::is_integral<U>::value > >
    static constexpr U eval() {
        return expr_evaluate<T, U>::fun::val;
    }
};

#include <stdio.h>

int main() {
   
    printf("%d\n", Fibin<unsigned int>::eval< Lit<Fib<1> > > () );
    Fibin<char>::eval< Lit<Fib<3> > > ();
    
    printf("%d\n", Fibin<unsigned int>::eval< 
    If< 
        If< 
            Lit<True>, 
            Lit<False>,
            Lit<True>
        >, 
        Lit<Fib<10>>, 
        Lit<Fib<2>> 
    > 
    >() );

    
    printf("%lu\n", Fibin<uint64_t>::eval<
        Invoke<
            Let<
                Var("x"), 
                Lit< Fib<0> >, 
                Lambda<
                    Var("x"), 
                    Ref<Var("x")>
                >
            >, 
            Lit< Fib<1> >
            >
        >());
      
    
    printf("%lu\n", Fibin<uint64_t>::eval<
        Let<
            Var("f"),
            Lambda<
                Var("x"),
                Ref<Var("x")>
            >,
            Invoke<Ref<Var("f")>, Lit<Fib<0> > >
        >
    >());
    printf("%d\n", Fibin<int>::eval<
        Let<
            Var("f"),
            Lambda<
                Var("x"),
                Inc1<Ref<Var("x")>>
            >,
            Invoke<Ref<Var("f")>, Lit<Fib<0> > >
        >
    >());
    printf("%d\n", Fibin<int>::eval<
        Let<
            Var("const"),
            Lit<Fib<1>>,
            Let<
                Var("f"),
                Lambda<
                    Var("x"),
                    Sum<
                        Ref<Var("const")>,
                        Ref<Var("x")>
                    >
                >,
                Let<
                    Var("const"),
                    Lit<Fib<3>>,
                    Invoke<
                        Ref<Var("f")>,
                        Lit<Fib<0>>
                    >
                >
            >
        >
    >());
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
