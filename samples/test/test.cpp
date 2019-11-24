//  test.cpp
//
//  Copyright (c) 2003 Eugene Gladyshev
//
//  Permission to copy, use, modify, sell and distribute this software
//  is granted provided this copyright notice appears in all copies.
//  This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.
//

#include <windows.h>
#if defined(_MSC_VER)
#include <crtdbg.h>
#endif
#include <iostream>
#include <stdarg.h>
#include <string>
#include <vector>

#include "ttl/flg/flags.hpp"
//#include "boost/bind.hpp"

#define BOOST_MPL_NO_PREPROCESSED_HEADERS
#undef BOOST_MPL_LIMIT_VECTOR_SIZE
#define BOOST_MPL_LIMIT_VECTOR_SIZE 25

//#define __USE_BOOST__

#if defined(__USE_BOOST__)
#include "boost/function.hpp"
#include "boost/signal.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/variant.hpp"
#include "boost/variant/apply_visitor.hpp"
namespace VAR = boost;
namespace SIG = boost;
namespace SIG_CON = boost::signals;
namespace FUNC = boost;
namespace TUP = boost::tuples;
typedef boost::signals::connection connection;
#else
#include "ttl/func/function.hpp"
#include "ttl/sig/signal.hpp"
#include "ttl/tup/tuple.hpp"
#include "ttl/var/variant.hpp"
namespace VAR = ttl::var;
namespace SIG = ttl::sig;
namespace SIG_CON = ttl::sig;
namespace FUNC = ttl::func;
namespace TUP = ttl::tup;
typedef ttl::sig::connection connection;
#endif

// define event types

typedef TUP::tuple<std::string, int> tuple_data;

enum myflags
{
    f1 = 1,
    f2 = 1 << 2,
    f3 = 1 << 3
};
typedef ttl::flg::flags<myflags> flags;

// event could contain one of the following:
//'double', 'int', 'tuple_data' or flags
typedef VAR::variant<double, int, tuple_data, flags> event;

// event visitor
#if !defined(__USE_BOOST__)
struct event_visitor
#else
struct event_visitor : boost::static_visitor<>
#endif
{
    void operator()(int x) const
    {
        std::cout << "int event: " << x << "\n";
    }
    void operator()(double x) const
    {
        std::cout << "double event: " << x << "\n";
    }
    void operator()(const tuple_data &x) const
    {
        std::cout << "tuple event: (\"" << TUP::get<0>(x) << "\", " << TUP::get<1>(x) << ")\n";
    }
    void operator()(const flags &x) const
    {
        std::cout << "flags event: ("
                  << "f1:" << x.test(f1) << ", f2:" << x.test(f2) << ", f3:" << x.test(f3) << ")\n";
    }
};

// event callback function
void event_callback(const event &ev)
{
    event_visitor visitor;
    VAR::apply_visitor(visitor, ev);
}

// other test functions
void named_params_test(); // implemented in named_params_test.cpp

void sccs_test();

void signal_test();

main()
{
#if defined(_MSC_VER)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    sccs_test();

    signal_test();

    // other test functions
    named_params_test();
}

void signal_test()
{
    // create signal and connect the same callback function
    // twice, directly and through ttl::func::function

    SIG::signal<void(event &)> sig;
    //	connection con1 = sig.connect( event_callback );
    FUNC::function<void(event &)> cb(event_callback);
    SIG_CON::connection con = sig.connect(cb);

    // signal int event
    event ev(10);
    sig(ev);

    // signal double event
    ev = 2.3;
    sig(ev);

    // signal flags event
    ev = flags(f2);
    sig(ev);

    ev = flags(f1, f3);
    sig(ev);

    // signal tuple_data event
    tuple_data td("some text", 99);
    const tuple_data &p = tuple_data("some text", 99);
    ev = tuple_data("some text", 99);
    sig(ev);

    con.disconnect();
}
