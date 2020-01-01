//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "common.hh"
#include "core.hh"

namespace drnsf {
namespace core {

// declared in core.hh
void argparser::parse(unsigned int &start, bool skip_deferred)
{
    continue_outer:
    while (start < m_argv.size()) {
        auto &arg = m_argv[start];

        // Early exit if the argument is not an option.
        if (arg.size() < 2 || arg[0] != '-' || arg == "--") {
            return;
        }

        if (arg[1] == '-') {
            // If the argument starts with "--" then it is a long option.

            // Find the "=" if present. This is for args such as "--foo=bar".
            auto longopt_eq_pos = arg.find('=');
            std::string longopt;
            std::string longopt_arg;
            bool longopt_arg_was_parsed;
            if (longopt_eq_pos != std::string::npos) {
                longopt = arg.substr(2, longopt_eq_pos - 2);
                longopt_arg = arg.substr(longopt_eq_pos + 1);
                longopt_arg_was_parsed = true;
            } else {
                longopt = arg.substr(2);
                longopt_arg_was_parsed = false;
            }

            // Find the option matching the one specified.
            auto longopt_it = m_longopts.find(longopt);
            if (longopt_it == m_longopts.end()) {
                throw arg_error("--$: not recognized"_fmt(longopt));
            }
            const auto &optinfo = *longopt_it->second;

            // Skip this option if it is deferred, unless we are processing all
            // deferred options now.
            if (skip_deferred && optinfo.deferred) {
                start++;

                // Skip the option's argument if needed and it wasn't provided
                // via "=", e.g. "--foo bar" instead of "--foo=bar".
                if (optinfo.takes_arg && !longopt_arg_was_parsed) {
                    if (start >= m_argv.size()) {
                        throw arg_error("--$: argument needed"_fmt(longopt));
                    }
                    start++;
                } else if (!optinfo.takes_arg && longopt_arg_was_parsed) {
                    throw arg_error("--$: unnecessary argument"_fmt(longopt));
                }

                continue;
            }

            // Consume the option.
            m_argv.erase(m_argv.begin() + start);

            // Grab the option's argument if needed and if it wasn't provided
            // via "=", e.g. "--foo bar" instead of "--foo=bar".
            if (optinfo.takes_arg && !longopt_arg_was_parsed) {
                if (start >= m_argv.size()) {
                    throw arg_error("--$: argument needed"_fmt(longopt));
                }
                longopt_arg = m_argv[start];

                // Consume the option's argument.
                m_argv.erase(m_argv.begin() + start);
            } else if (!optinfo.takes_arg && longopt_arg_was_parsed) {
                throw arg_error("--$: unnecessary argument"_fmt(longopt));
            }

            // Call the handler for this option.
            optinfo.f(longopt_arg);
        } else {
            // Otherwise, it is a short option set, e.g. "-a" or "-abc".

            // Process each character in this set.
            unsigned int i;
            for (i = 1; i < arg.size();) {
                char shortopt = arg[i];

                // Find the option matching the one specified.
                auto shortopt_it = m_shortopts.find(shortopt);
                if (shortopt_it == m_shortopts.end()) {
                    throw arg_error("-$: not recognized"_fmt(shortopt));
                }
                const auto &optinfo = *shortopt_it->second;

                // Skip this option if it is deferred, unless we are processing
                // all deferred options now.
                if (skip_deferred && optinfo.deferred) {
                    i++;

                    // Skip this option's argument if needed. This may be joint
                    // form (e.g. "-fhello.txt") or separate ("-f hello.txt").
                    if (optinfo.takes_arg) {
                        if (i >= arg.size()) {
                            // Separate form.
                            start++;
                            if (start >= m_argv.size()) {
                                throw arg_error(
                                    "-$: argument needed"_fmt(shortopt)
                                );
                            }
                            start++;
                            goto continue_outer;
                        } else {
                            // Joint form.
                            start++;
                            goto continue_outer;
                        }
                    }

                    continue;
                }

                // Consume the option.
                arg.erase(i, 1);

                // Grab the option's argument if it takes one. This may be joint
                // form (e.g. "-fhello.txt") or separate ("-f hello.txt").
                std::string shortopt_arg;
                if (optinfo.takes_arg) {
                    if (i >= arg.size()) {
                        // Separate form.
                        if (start + 1 >= m_argv.size()) {
                            throw arg_error(
                                "-$: argument needed"_fmt(shortopt)
                            );
                        }
                        shortopt_arg = m_argv[start + 1];
                        m_argv.erase(m_argv.begin() + start + 1);
                    } else {
                        // Joint form.
                        shortopt_arg = arg.substr(i);
                        arg.erase(i);
                    }
                }

                // Call the handler for this option.
                optinfo.f(shortopt_arg);

                if (optinfo.takes_arg) {
                    // When handling separate-form arguments for shortopts,
                    // `arg' becomes invalidated by the `erase' call. For this
                    // reason, we must break out manually rather than rely on
                    // the loop condition.
                    break;
                }
            }

            // Consume this argument if there are no options left ("-").
            if (i == 1) {
                m_argv.erase(m_argv.begin() + start);
            } else {
                start++;
            }
        }
    }
}

// declared in core.hh
void argparser::begin(argv_t argv)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    m_argv = std::move(argv);

    // Immediately parse the argument list and consume/process the non-deferred
    // options found.
    for (unsigned int i = 0; i < m_argv.size(); i++) {
        parse(i, true);

        // Stop parsing when the terminator is reached (if present).
        if (i < m_argv.size() && m_argv[i] == "--") {
            break;
        }
    }

    m_active = true;
    m_terminated = false;
}

// declared in core.hh
void argparser::end()
{
    // Ensure the parser is already running.
    if (!m_active) {
        throw std::logic_error("core::argparser: not active");
    }

    // Finish processing all remaining arguments. If there are non-option
    // arguments remaining, raise an error.
    if (!pump_eof()) {
        throw arg_error("too many arguments");
    }

    m_active = false;
}

// declared in core.hh
void argparser::add_opt(
    char shortopt,
    std::function<void()> f,
    bool deferred)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the option doesn't already exist.
    if (m_shortopts.find(shortopt) != m_shortopts.end()) {
        throw std::logic_error("core::argparser: shortopt already exists");
    }

    // Build the information for this option.
    opt o;
    o.takes_arg = false;
    o.deferred = deferred;
    o.f = [f](std::string s) { f(); };
    m_opts.push_back(std::move(o));
    m_shortopts.insert({shortopt, &m_opts.back()});
}

// declared in core.hh
void argparser::add_opt(
    std::string longopt,
    std::function<void()> f,
    bool deferred)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the option doesn't already exist.
    if (m_longopts.find(longopt) != m_longopts.end()) {
        throw std::logic_error("core::argparser: longopt already exists");
    }

    // Build the information for this option.
    opt o;
    o.takes_arg = false;
    o.deferred = deferred;
    o.f = [f](std::string s) { f(); };
    m_opts.push_back(std::move(o));
    m_longopts.insert({std::move(longopt), &m_opts.back()});
}

// declared in core.hh
void argparser::add_opt(
    char shortopt,
    std::function<void(std::string)> f,
    bool deferred)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the option doesn't already exist.
    if (m_shortopts.find(shortopt) != m_shortopts.end()) {
        throw std::logic_error("core::argparser: shortopt already exists");
    }

    // Build the information for this option.
    opt o;
    o.takes_arg = true;
    o.deferred = deferred;
    o.f = std::move(f);
    m_opts.push_back(std::move(o));
    m_shortopts.insert({shortopt, &m_opts.back()});
}

// declared in core.hh
void argparser::add_opt(
    std::string longopt,
    std::function<void(std::string)> f,
    bool deferred)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the option doesn't already exist.
    if (m_longopts.find(longopt) != m_longopts.end()) {
        throw std::logic_error("core::argparser: longopt already exists");
    }

    // Build the information for this option.
    opt o;
    o.takes_arg = true;
    o.deferred = deferred;
    o.f = std::move(f);
    m_opts.push_back(std::move(o));
    m_longopts.insert({std::move(longopt), &m_opts.back()});
}

// declared in core.hh
void argparser::alias_opt(char alias, char target)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the alias option doesn't already exist.
    if (m_shortopts.find(alias) != m_shortopts.end()) {
        throw std::logic_error("core::argparser: shortopt already exists");
    }

    // Find the target option.
    auto it = m_shortopts.find(target);
    if (it == m_shortopts.end()) {
        throw std::logic_error("core::argparser: shortopt does not exist");
    }

    // Apply the alias.
    m_shortopts.insert({alias, it->second});
}

// declared in core.hh
void argparser::alias_opt(char alias, std::string target)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the alias option doesn't already exist.
    if (m_shortopts.find(alias) != m_shortopts.end()) {
        throw std::logic_error("core::argparser: shortopt already exists");
    }

    // Find the target option.
    auto it = m_longopts.find(target);
    if (it == m_longopts.end()) {
        throw std::logic_error("core::argparser: longopt does not exist");
    }

    // Apply the alias.
    m_shortopts.insert({alias, it->second});
}

// declared in core.hh
void argparser::alias_opt(std::string alias, char target)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the alias option doesn't already exist.
    if (m_longopts.find(alias) != m_longopts.end()) {
        throw std::logic_error("core::argparser: longopt already exists");
    }

    // Find the target option.
    auto it = m_shortopts.find(target);
    if (it == m_shortopts.end()) {
        throw std::logic_error("core::argparser: shortopt does not exist");
    }

    // Apply the alias.
    m_longopts.insert({std::move(alias), it->second});
}

// declared in core.hh
void argparser::alias_opt(std::string alias, std::string target)
{
    // Ensure the parser hasn't already started.
    if (m_active) {
        throw std::logic_error("core::argparser: already active");
    }

    // Ensure the alias option doesn't already exist.
    if (m_longopts.find(alias) != m_longopts.end()) {
        throw std::logic_error("core::argparser: longopt already exists");
    }

    // Find the target option.
    auto it = m_longopts.find(target);
    if (it == m_longopts.end()) {
        throw std::logic_error("core::argparser: longopt does not exist");
    }

    // Apply the alias.
    m_longopts.insert({std::move(alias), it->second});
}

// declared in core.hh
bool argparser::pump_eof()
{
    // Ensure the parser is already running.
    if (!m_active) {
        throw std::logic_error("core::argparser: not active");
    }

    if (!m_terminated) {
        // Parse and process deferred options up to the next argument, or the
        // terminator ("--") if present.
        unsigned int i = 0;
        parse(i, false);

        // Handle the terminator if met.
        if (!m_argv.empty() && m_argv[0] == "--") {
            m_argv.pop_front();
            m_terminated = true;
        }
    }

    return m_argv.empty();
}

// declared in core.hh
argparser &operator >>(argparser &lhs, std::string &rhs)
{
    // Ensure the parser is already running.
    if (!lhs.m_active) {
        throw std::logic_error("core::argparser: not active");
    }

    // Process all deferred options up to the next argument. Fail if there are
    // no arguments remaining to be shifted.
    if (lhs.pump_eof()) {
        throw arg_error("not enough arguments");
    }

    // Pop the front argument into the shift target.
    rhs = lhs.m_argv.front();
    lhs.m_argv.pop_front();

    return lhs;
}

#if FEATURE_INTERNAL_TEST
namespace {

TEST(core_argparser, ShortOptSeparate)
{
    argv_t argv = { "-a", "-b", "-c", "-f", "optarg", "-v", "-v", "-v", "arg" };
    bool a_ok = false;
    bool b_ok = false;
    bool c_ok = false;
    std::string f_name = "bad";
    int v_count = 0;

    argparser o;
    o.add_opt('a', [&]{ a_ok = true; }, true);
    o.add_opt('b', [&]{ b_ok = true; });
    o.add_opt('c', [&]{ c_ok = true; }, true);
    o.add_opt('f', [&](std::string s) { f_name = s; });
    o.add_opt('v', [&]{ v_count++; });
    EXPECT_FALSE(a_ok);
    EXPECT_FALSE(b_ok);
    EXPECT_FALSE(c_ok);
    EXPECT_EQ(f_name, "bad");
    EXPECT_EQ(v_count, 0);

    o.begin(argv);
    EXPECT_FALSE(a_ok);
    EXPECT_TRUE(b_ok);
    EXPECT_FALSE(c_ok);
    EXPECT_EQ(f_name, "optarg");
    EXPECT_EQ(v_count, 3);

    std::string arg;
    o >> arg;
    EXPECT_EQ(arg, "arg");
    EXPECT_TRUE(a_ok);
    EXPECT_TRUE(b_ok);
    EXPECT_TRUE(c_ok);
    EXPECT_EQ(f_name, "optarg");
    EXPECT_EQ(v_count, 3);

    o.end();
}

TEST(core_argparser, ShortOptJoint)
{
    argv_t argv = { "-abcfoptarg", "-vvv", "arg" };
    bool a_ok = false;
    bool b_ok = false;
    bool c_ok = false;
    std::string f_name = "bad";
    int v_count = 0;

    argparser o;
    o.add_opt('a', [&]{ a_ok = true; }, true);
    o.add_opt('b', [&]{ b_ok = true; });
    o.add_opt('c', [&]{ c_ok = true; }, true);
    o.add_opt('f', [&](std::string s) { f_name = s; });
    o.add_opt('v', [&]{ v_count++; });
    EXPECT_FALSE(a_ok);
    EXPECT_FALSE(b_ok);
    EXPECT_FALSE(c_ok);
    EXPECT_EQ(f_name, "bad");
    EXPECT_EQ(v_count, 0);

    o.begin(argv);
    EXPECT_FALSE(a_ok);
    EXPECT_TRUE(b_ok);
    EXPECT_FALSE(c_ok);
    EXPECT_EQ(f_name, "optarg");
    EXPECT_EQ(v_count, 3);

    std::string arg;
    o >> arg;
    EXPECT_EQ(arg, "arg");
    EXPECT_TRUE(a_ok);
    EXPECT_TRUE(b_ok);
    EXPECT_TRUE(c_ok);
    EXPECT_EQ(f_name, "optarg");
    EXPECT_EQ(v_count, 3);

    o.end();
}

TEST(core_argparser, LongOpt)
{
    argv_t argv = {
        "--alpha",
        "--bravo",
        "--charlie",
        "--file",
        "optarg",
        "--verbose",
        "--verbose",
        "--verbose",
        "arg"
    };
    bool a_ok = false;
    bool b_ok = false;
    bool c_ok = false;
    std::string f_name = "bad";
    int v_count = 0;

    argparser o;
    o.add_opt("alpha", [&]{ a_ok = true; }, true);
    o.add_opt("bravo", [&]{ b_ok = true; });
    o.add_opt("charlie", [&]{ c_ok = true; }, true);
    o.add_opt("file", [&](std::string s) { f_name = s; });
    o.add_opt("verbose", [&]{ v_count++; });
    EXPECT_FALSE(a_ok);
    EXPECT_FALSE(b_ok);
    EXPECT_FALSE(c_ok);
    EXPECT_EQ(f_name, "bad");
    EXPECT_EQ(v_count, 0);

    o.begin(argv);
    EXPECT_FALSE(a_ok);
    EXPECT_TRUE(b_ok);
    EXPECT_FALSE(c_ok);
    EXPECT_EQ(f_name, "optarg");
    EXPECT_EQ(v_count, 3);

    std::string arg;
    o >> arg;
    EXPECT_EQ(arg, "arg");
    EXPECT_TRUE(a_ok);
    EXPECT_TRUE(b_ok);
    EXPECT_TRUE(c_ok);
    EXPECT_EQ(f_name, "optarg");
    EXPECT_EQ(v_count, 3);

    o.end();
}

TEST(core_argparser, DoubleBeginError)
{
    argv_t argv;
    argparser o;
    o.begin(argv);
    EXPECT_THROW(o.begin(argv), std::logic_error);
}

TEST(core_argparser, EarlyEndError)
{
    argv_t argv = { "test" };
    argparser o;
    o.begin(argv);
    EXPECT_THROW(o.end(), arg_error);
}

TEST(core_argparser, DoubleEndError)
{
    argv_t argv;
    argparser o;
    o.begin(argv);
    o.end();
    EXPECT_THROW(o.end(), std::logic_error);
}

TEST(core_argparser, OverreadError)
{
    argv_t argv = { "test" };
    argparser o;
    o.begin(argv);
    std::string s;
    o >> s;
    EXPECT_THROW(o >> s, arg_error);
}

TEST(core_argparser, AddOptDuringOperationError)
{
    argv_t argv = { "test" };
    argparser o;
    o.begin(argv);
    EXPECT_THROW(o.add_opt('x', []{}), std::logic_error);
}

TEST(core_argparser, DuplicateOptError)
{
    argparser o;
    o.add_opt('x', []{});
    EXPECT_THROW(o.add_opt('x', []{}), std::logic_error);
}

TEST(core_argparser, MissingOptError)
{
    argv_t argv = { "-x" };
    argparser o;
    EXPECT_THROW(o.begin(argv), arg_error);
}

TEST(core_argparser, Terminator)
{
    argv_t argv = { "-x", "--", "-yz" };
    bool x_ok = false;
    bool y_ok = true;
    bool z_ok = true;

    argparser o;
    o.add_opt('x', [&]{ x_ok = true; });
    o.add_opt('y', [&]{ y_ok = false; }, true);
    o.add_opt('z', [&]{ z_ok = false; });
    o.begin(argv);
    EXPECT_TRUE(x_ok);
    EXPECT_TRUE(y_ok);
    EXPECT_TRUE(z_ok);

    std::string arg;
    o >> arg;
    EXPECT_TRUE(o.pump_eof());
    EXPECT_EQ(arg, "-yz");
    EXPECT_TRUE(x_ok);
    EXPECT_TRUE(y_ok);
    EXPECT_TRUE(z_ok);

    o.end();
}

TEST(core_argparser, DeferredWithArg)
{
    argv_t argv = { "-fONE", "arg", "-f", "TWO" };
    bool o_ok = true;
    std::string f_name = "";

    argparser o;
    o.add_opt('O', [&]{ o_ok = false; });
    o.add_opt('f', [&](std::string s) { f_name = s; }, true);

    o.begin(argv);
    EXPECT_TRUE(o_ok);
    EXPECT_EQ(f_name, "");

    std::string arg;
    o >> arg;
    EXPECT_EQ(arg, "arg");
    EXPECT_TRUE(o_ok);
    EXPECT_EQ(f_name, "ONE");

    EXPECT_TRUE(o.pump_eof());
    EXPECT_TRUE(o_ok);
    EXPECT_EQ(f_name, "TWO");

    o.end();
}

}
#endif

}
}
