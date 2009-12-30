/*
 * tut_xml_reporter.hpp
 *
 * ECOS Library. IPT CS R&D CET ECOS Copyright 2008 Nokia
 * Siemens Networks. All right
 *
 *
 */

#ifndef TUT_XML_REPORTER
#define TUT_XML_REPORTER

#include <tut/tut.hpp>
#include <string>
#include <fstream>
#include <vector>
#include <stdexcept>

namespace tut
{

/**
 * \brief JUnit XML TUT reporter
 * @author Lukasz Maszczynski, NSN
 * @date 11/07/2008
 */
class xml_reporter : public tut::callback
{
protected:
    typedef std::vector<tut::test_result> TestResults;
    typedef std::map<std::string, TestResults> TestGroups;

    TestGroups all_tests; /// holds all test results
    std::string filename; /// filename base

    /**
     * \brief Initializes object
     * Resets counters and clears all stored test results.
     */
    virtual void init()
    {
        ok_count = 0;
        exceptions_count = 0;
        failures_count = 0;
        terminations_count = 0;
        warnings_count = 0;
        all_tests.clear();
    }

    /**
     * \brief Encodes text to XML
     * XML-reserved characters (e.g. "<") are encoded according to specification
     * @param text text to be encoded
     * @return encoded string
     */
    virtual std::string encode(const std::string & text)
    {
        std::string out;

        for (unsigned int i=0; i<text.length(); ++i) {
            char c = text[i];
            switch (c) {
                case '<':
                    out += "&lt;";
                    break;
                case '>':
                    out += "&gt;";
                    break;
                case '&':
                    out += "&amp;";
                    break;
                case '\'':
                    out += "&apos;";
                    break;
                case '"':
                    out += "&quot;";
                    break;
                default:
                    out += c;
            }
        }

        return out;
    }

    /**
     * \brief Builds "testcase" XML entity with given parameters
     * Builds \<testcase\> entity according to given parameters. \<testcase\>-s are part of \<testsuite\>.
     * @param tr test result to be used as source data
     * @param failure_type type of failure to be reported ("Assertion" or "Error", empty if test passed)
     * @param failure_msg failure message to be reported (empty, if test passed)
     * @return string with \<testcase\> entity
     */
    virtual std::string xml_build_testcase(const tut::test_result & tr, const std::string & failure_type,
                                           const std::string & failure_msg, int pid = 0)
    {
        using std::endl;
        using std::string;

        std::ostringstream out;

        if (tr.result == test_result::ok)
        {
            out << "  <testcase classname=\"" << encode(tr.group) << "\" name=\"" << encode(tr.name) << "\" />";
        }
        else
        {
            string err_msg = encode(failure_msg + tr.message);

            string tag; // determines tag name: "failure" or "error"
            if ( tr.result == test_result::fail || tr.result == test_result::warn ||
                 tr.result == test_result::ex || tr.result == test_result::ex_ctor )
            {
                tag = "failure";
            }
            else
            {
                tag = "error";
            }

            out << "  <testcase classname=\"" << encode(tr.group) << "\" name=\"" << encode(tr.name) << "\">" << endl;
            out << "    <" << tag << " message=\"" << err_msg << "\"" << " type=\"" << failure_type << "\"";
#if defined(TUT_USE_POSIX)
            if(pid != getpid())
            {
                out << " child=\"" << pid << "\"";
            }
#endif
            out << ">" << err_msg << "</" << tag << ">" << endl;
            out << "  </testcase>";
        }

        return out.str();
    }

    /**
     * \brief Builds "testsuite" XML entity
     * Builds \<testsuite\> XML entity according to given parameters.
     * @param errors number of errors to be reported
     * @param failures number of failures to be reported
     * @param total total number of tests to be reported
     * @param name test suite name
     * @param testcases encoded XML string containing testcases
     * @return string with \<testsuite\> entity
     */
    virtual std::string xml_build_testsuite(int errors, int failures, int total,
                                            const std::string & name, const std::string & testcases)
    {
        std::ostringstream out;

        out << "<testsuite errors=\"" << errors << "\" failures=\"" << failures << "\" tests=\"" << total << "\" name=\"" << encode(name) << "\">" << std::endl;
        out << testcases;
        out << "</testsuite>";

        return out.str();
    }

public:
    int ok_count;           /// number of passed tests
    int exceptions_count;   /// number of tests that threw exceptions
    int failures_count;     /// number of tests that failed
    int terminations_count; /// number of tests that would terminate
    int warnings_count;     /// number of tests where destructors threw an exception

    /**
     * \brief Default constructor
     * @param filename base filename
     * @see setFilenameBase
     */
    xml_reporter(const std::string & _filename = "")
    {
        init();
        setFilenameBase(_filename);
    }

    /**
     * \brief Sets filename base for output
     * @param _filename filename base
     * Example usage:
     * @code
     * xml_reporter reporter;
     * reporter.setFilenameBase("my_xml");
     * @endcode
     * The above code will instruct reporter to create my_xml_1.xml file for the first test group,
     * my_xml_2.xml file for the second, and so on.
     */
    virtual void setFilenameBase(const std::string & _filename)
    {
        if (_filename == "")
        {
            filename = "testResult";
        }
        else
        {
            if (_filename.length() > 200)
            {
                throw(std::runtime_error("Filename too long!"));
            }

            filename = _filename;
        }
    }

    /**
     * \brief Callback function
     * This function is called before the first test is executed. It initializes counters.
     */
    virtual void run_started()
    {
        init();
    }

    /**
     * \brief Callback function
     * This function is called when test completes. Counters are updated here, and test results stored.
     */
    virtual void test_completed(const tut::test_result& tr)
    {
        // update global statistics
        switch (tr.result) {
            case test_result::ok:
                ok_count++;
                break;
            case test_result::fail:
            case test_result::rethrown:
                failures_count++;
                break;
            case test_result::ex:
            case test_result::ex_ctor:
                exceptions_count++;
                break;
            case test_result::warn:
                warnings_count++;
                break;
            case test_result::term:
                terminations_count++;
                break;
        } // switch

        // add test result to results table
        (all_tests[tr.group]).push_back(tr);
    }

    /**
     * \brief Callback function
     * This function is called when all tests are completed. It generates XML output
     * to file(s). File name base can be set with \ref setFilenameBase.
     */
    virtual void run_completed()
    {
        using std::endl;
        using std::string;

        static int number = 1;  // results file sequence number (testResult_<number>.xml)

        // iterate over all test groups
        TestGroups::const_iterator tgi;
        for (tgi = all_tests.begin(); tgi != all_tests.end(); ++tgi) {
            /* per-group statistics */
            int passed = 0;         // passed in single group
            int exceptions = 0;     // exceptions in single group
            int failures = 0;       // failures in single group
            int terminations = 0;   // terminations in single group
            int warnings = 0;       // warnings in single group
            int errors = 0;     // errors in single group

            /* generate output filename */
            char fn[256];
            sprintf(fn, "%s_%d.xml", filename.c_str(), number++);

            std::ofstream xmlfile;
            xmlfile.open(fn, std::ios::in | std::ios::trunc);
            if (!xmlfile.is_open()) {
                throw (std::runtime_error("Cannot open file for output"));
            }

            /* *********************** header ***************************** */
            xmlfile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;

            // output is written to string stream buffer, because JUnit format <testsuite> tag
            // contains statistics, which aren't known yet
            std::ostringstream out;

            // iterate over all test cases in the current test group
            TestResults::const_iterator tri;
            for (tri = (*tgi).second.begin(); tri != (*tgi).second.end(); ++tri) {
                string failure_type;    // string describing the failure type
                string failure_msg;     // a string with failure message

                switch ((*tri).result) {
                    case test_result::ok:
                        passed++;
                        break;
                    case test_result::fail:
                        failure_type = "Assertion";
                        failure_msg  = "";
                        failures++;
                        break;
                    case test_result::ex:
                        failure_type = "Assertion";
                        failure_msg  = "Thrown exception: " + (*tri).exception_typeid + '\n';
                        exceptions++;
                        break;
                    case test_result::warn:
                        failure_type = "Assertion";
                        failure_msg  = "Destructor failed.\n";
                        warnings++;
                        break;
                    case test_result::term:
                        failure_type = "Error";
                        failure_msg  = "Test application terminated abnormally.\n";
                        terminations++;
                        break;
                    case test_result::ex_ctor:
                        failure_type = "Assertion";
                        failure_msg  = "Constructor has thrown an exception: " + (*tri).exception_typeid + '\n';
                        exceptions++;
                        break;
                    case test_result::rethrown:
                        failure_type = "Assertion";
                        failure_msg  = "Child failed";
                        failures++;
                        break;
                    default:
                        failure_type = "Error";
                        failure_msg  = "Unknown test status, this should have never happened. "
                                "You may just have found a BUG in TUT XML reporter, please report it immediately.\n";
                        errors++;
                        break;
                } // switch

#if defined(TUT_USE_POSIX)
                out << xml_build_testcase(*tri, failure_type, failure_msg, (*tri).pid) << endl;
#else
                out << xml_build_testcase(*tri, failure_type, failure_msg) << endl;
#endif

            } // iterate over all test cases

            // calculate per-group statistics
            int stat_errors = terminations + errors;
            int stat_failures = failures + warnings + exceptions;
            int stat_all = stat_errors + stat_failures + passed;

            xmlfile << xml_build_testsuite(stat_errors, stat_failures, stat_all, (*tgi).first/* name */, out.str()/* testcases */) << endl;
            xmlfile.close();
        } // iterate over all test groups
    }

    /**
     * \brief Returns true, if all tests passed
     */
    virtual bool all_ok() const
    {
        return ( (terminations_count + failures_count + warnings_count + exceptions_count) == 0);
    };
};

}

#endif
