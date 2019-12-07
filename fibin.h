#include <cstdint>
#include <cassert>

#include <typeinfo>
#include <iostream>

#define NO_VARIABLE 0
using VarID = uint32_t;

template<VarID id, typename Value> 
struct Variable { };

////////////////////

template<typename T, T _value>
struct Value{ 
    using type = T;
    static constexpr type value = _value;
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
    static constexpr typename T::type val = T::value;
};

template<bool _logical, typename... context> 
struct Function<NO_VARIABLE, Value<bool, _logical>, context...>{
    static constexpr bool logical = _logical;
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

template<typename Expression, typename... context> struct Expr_evaluate;

template<typename... Args, template<typename...> typename Expression, typename... context >
struct Expr_evaluate<Expression<Args...>, context...> {
    using fun = typename Expression<Args..., context...>::fun;
};

template< VarID id, typename... Args, template<VarID, typename...> typename Expression, typename... context >
struct Expr_evaluate<Expression<id, Args...>, context...> {
    using fun = typename Expression<id, Args..., context...>::fun;
};

////////////////////

template<typename T> struct Evaluate;

template<VarID id, typename Body, typename... context>
struct Evaluate<Function<id, Body, context...>> {
    using fun = typename Expr_evaluate <Body, context...>::fun;
};

////////////////////

#include <type_traits>

template<typename Condition, typename IfTrue, typename IfFalse, typename... context> 
struct If {
    using fun = typename Expr_evaluate<
        typename std::conditional <
            Expr_evaluate <Condition, context...>::fun::logical,
            IfTrue, 
            IfFalse
        >::type,
        context...
    >::fun;
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
                (Expr_evaluate<Left, context...>::fun::val == Expr_evaluate<Right, context...>::fun::val)
            >,
            context...
        >;
};

////////////////////

template <VarID id, typename Value, typename T, typename... context> 
struct Let {
    using fun = 
        typename Expr_evaluate<
            T, 
            Variable<
                id, 
                Expr_evaluate<Value, context...>
            >, context...
        >::fun;
};

template<typename Value, typename T, typename... context> 
struct Let<NO_VARIABLE, Value, T, context...>;

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

template<typename... context>
struct Lookup<NO_VARIABLE, context...>;

////////////////////

template <VarID Var, typename... context>
using Ref = Lookup<Var, context...>;

////////////////////

template <VarID Var, typename Body, typename... context>
struct Lambda { 
    using fun = Function<Var, Body, context...>;
};

template<typename Body, typename... context>
struct Lambda<NO_VARIABLE, Body, context...>;

////////////////////

template <typename Fun, typename Param, typename... context>
struct Invoke {
    using fun =
        typename Evaluate< 
            typename enrich< 
                typename Expr_evaluate<Fun, context...>::fun, 
                Expr_evaluate<Param, context...>
            >::fun
        >::fun;
};

////////////////////

template<typename Arg, typename... context> 
struct Inc1 {
    using numeric_ = typename Get_numeric<context...>::type;
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                numeric_,
                static_cast<numeric_>
                    (Expr_evaluate<Arg, context...>::fun::val + 
                     Fib_eval<1, numeric_>::val)
            >,
            context...
        >;
};

template<typename Arg, typename... context> 
struct Inc10 {
    using numeric_ = typename Get_numeric<context...>::type;
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                numeric_,
                static_cast<numeric_>
                    (Expr_evaluate<Arg, context...>::fun::val + 
                     Fib_eval<10, numeric_>::val)
            >,
            context...
        >;
};

////////////////////

template<typename... Args> struct Sum;

template<typename T, typename... Args>
struct Sum<T, Args...> {
    using numeric_ = typename Get_numeric<Args...>::type;
    using fun = 
        Function<
            NO_VARIABLE, 
            Value<
                numeric_,
                static_cast<numeric_>
                    (Expr_evaluate<T, Args...>::fun::val + 
                     Sum<Args...>::fun::val)
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
                Expr_evaluate<T, Number>::fun::val
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
                Expr_evaluate<T, Variable<id, Val>, Args...>::fun::val
            >,
            Args...
        >;
};


////////////////////

constexpr VarID Var(const char *name) {
    VarID returned = NO_VARIABLE + 1;
    
    int i = 0; 
    while(i <= 6 && name[i] != 0) {
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
        VarID result = 0;
        VarID factor = 1;
        
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

template< typename Number >
class Fibin {
    public:
    
    template< typename T, typename U = Number, typename = typename std::enable_if_t<!std::is_integral<U>::value > >
    static void eval() {
        std::cout << "Fibin doesn't support " << typeid(Number).name() << "!\n";
    }
    
    template< typename T, typename U = Number, typename = typename std::enable_if_t<std::is_integral<U>::value > >
    static constexpr U eval() {
        return Expr_evaluate<T, U>::fun::val;
    }
};
