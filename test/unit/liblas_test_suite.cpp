// $Id$
//
// (C) Copyright Mateusz Loskot 2008, mateusz@loskot.net
// Distributed under the BSD License
// (See accompanying file LICENSE.txt or copy at
// http://www.opensource.org/licenses/bsd-license.php)
//
#include <tut/tut.hpp>
#include <tut/tut_reporter.hpp>
#include <iostream>

namespace tut
{
    test_runner_singleton runner;
}

int main()
{
    tut::reporter reporter;
    tut::runner.get().set_callback(&reporter);

    try
    {
        tut::runner.get().run_tests();
    }
    catch (std::exception const& e)
    {
        std::cerr << "TUT raised exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

