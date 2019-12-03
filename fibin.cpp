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

template<VarID id, typename T, typename... context>
struct Function { };

template<typename T, typename... context> 
struct Function<NO_VARIABLE, T, context...>{
    static constexpr auto val = T::value;
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

template<typename T, typename... context>
struct expr_evaluate {};

template< typename... Targs, template<typename...> typename T, typename... context >
struct expr_evaluate<T<Targs...>, context...> {
    static constexpr auto val = T<Targs..., context...>::val;
};

template< int i, typename... Targs, template<int, typename...> typename T, typename... context >
struct expr_evaluate<T<i, Targs...>, context...> {
    static constexpr auto val = T<i, Targs..., context...>::val;
};

template<VarID id, typename Body, typename... context>
struct evaluate<Function<id, Body, context...> {
    using fun = evaluate_expr<Body, context...>::val;
}

////////////////////

template<typename Condition, typename IfTrue, typename IfFalse, typename... context> 
struct If {
    static constexpr auto val = 
        expr_evaluate <Condition, context...>::val ?
        expr_evaluate <IfTrue, context...>::val :
        expr_evaluate <IfFalse, context...>::val;
};

////////////////////

template<typename Function, typename new>
struct enrich { };

template<VarID id, typename Body, typename... context, template<VarID, typename> typename Variable>
struct enrich<Function<id, Body, context...>,  Variable<id, Value>> {
   using fun = Function<id, Body, Variable<id, Value>, context>;
}

///////////////////

template<typename Left, typename Right, typename... context>
struct Eq {
    static constexpr bool val = 
        (expr_evaluate<Left, context...>::val == expr_evaluate<Right, context...>::val);
};

////////////////////

template <VarID id, typename Value, typename T, typename... context> 
struct Let() {
    using fun = enrich<T::fun, Variable<id, Value>>;
}

template <VarID Var, typename Value, typename Expression, typename... context> 
struct Let { 
    static constexpr bool val =
        expr_evaluate<Expression, Variable<Var, Value>, context...>::val; 
};

////////////////////

#include <type_traits>

template<VarID Var, typename... context>
struct Lookup_expr_evaluate { };

template<VarID Var, VarID id, typename Value, template<VarID, typename> typename Variable, typename... context> 
struct Lookup_expr_evaluate<Var, Variable<id, Value>, context...> {
    static constexpr auto val = 
        Var == id ?
        expr_evaluate<Value, context...>::val :
        Lookup_expr_evaluate<Var, context...>::val;
};

////////////////////

template <VarID Var, typename... context>
struct Ref {
    static constexpr auto val = Lookup_expr_evaluate<Var, context...>::val;
};

////////////////////

template <VarID Var, typename Body, typename... context>
struct Lambda { 
    using fun = Function<id, Body, context...>;
};

////////////////////

template <typename Fun, typename Param, typename... context>
struct Invoke { };

template < VarID id, typename Body, typename Fun, typename... context>
struct Invoke<> {
    using fun = evaluate<enrich<Fun::fun, Variable<id, Body>>>::fun;
}

////////////////////

template<typename Arg, typename... context> 
struct Inc1 {
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                typename Get_numeric<context...>::type, 
                expr_evaluate<Arg, context...>::val + 1
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
                expr_evaluate<Arg, context...>::val + 10
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
                expr_evaluate<T, Args...>::val + Sum<Args...>::fun::val
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
                expr_evaluate<T, Number>::val
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
                expr_evaluate<T, Args...>::val
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
        return expr_evaluate<T, U>::val;
    }
};

#include <stdio.h>

int main() {
   
    printf("%d\n", Fibin<unsigned int>::eval< Lit<Fib<1> > > () );
    // Fibin<char>::eval< Lit<Fib<3> > > ();
    
    // printf("%d\n", Fibin<unsigned int>::eval< 
    // If< 
    //     If< 
    //         Lit<True>, 
    //         Lit<False>,
    //         Lit<True>
    //     >, 
    //     Lit<Fib<10>>, 
    //     Lit<Fib<2>> 
    // > 
    // >() );

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
