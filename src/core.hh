//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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

#pragma once

/*
 * core.hh
 *
 * FIXME explain
 */

#include <deque>

namespace drnsf {
namespace core {

/*
 * core::argv_t
 *
 * This container type is intended to replace the C-style argc/argv structure.
 * It is built using a deque to allow easier removal from the front and middle
 * of the argument list.
 */
using argv_t = std::deque<std::string>;

/*
 * core::arg_error
 *
 * This is an exception type. Objects of this type are thrown by the argparser
 * (see below) when it encounters an error while parsing options from a list of
 * arguments.
 *
 * For example:
 *
 *   -v: option not recognized
 *   -f: argument required
 *   --enable-things: unnecessary argument
 *
 * This is not thrown by other errors necessarily caused by misuse of the
 * parser, such as registering multiple options with the same name, or starting
 * on a second argument list without completing the previous one.
 */
class arg_error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

/*
 * core::argparser
 *
 * FIXME explain
 *
 * Some examples of supported scenarios:
 *
 *   -a -b -c
 *   -abc
 *     The shortopt handlers for 'a', 'b', and 'c' will be called in order.
 *
 *   -f nicefile.dat
 *   -fnicefile.dat
 *     The shortopt handler for 'f' will be called with "nicefile.dat" as the
 *     argument.
 *
 *   -v -v -v
 *   -vvv
 *     The shortopt handler for 'v' will be called three times.
 *
 *   --foo --bar --etc
 *     The longopt handlers for "foo", "bar", and "etc" will be called in
 *     order.
 *
 *   --file hello.txt
 *   --file=hello.txt
 *     The longopt handler for "file" will be called with "hello.txt" as the
 *     argument.
 *
 *   --file a.dat --file b.dat --file c.dat
 *     The longopt handler for "file" will be called with "a.dat", then again
 *     with "b.dat", and again with "c.dat".
 *
 *   --foo --bar --etc -- --strange-filename--
 *     The longopt handlers for "foo", "bar", and "etc" will be called in
 *     order. "--strange-filename--" will not be parsed as an option because
 *     it appears after a "--" argument.
 *
 * Each option may be "deferred" or "non-deferred". Non-deferred options are
 * processed immediately when the parser is started with `begin'. This is
 * intended for options which need to apply equally or immediately regardless of
 * their position in a command line. For example, imagine the following command:
 *
 * ./drnsf :delete-lots-of-files file1.txt file2.txt file3.txt --help
 *
 * In the above case, it is very important that the "--help" option take effect
 * before parsing and deleting the 3 given files.
 *
 * Deferred options are processed as they are encountered during shifting of
 * arguments (`operator >>'), calls to `pump_eof', and the call to `end'. This
 * is intended for options which should only apply to arguments specified after
 * the option is given, for example:
 *
 * ./drnsf :apply-colors-to-things --color=red A B C --color=blue X Y Z
 *
 * With "--color" as a deferred option which takes one argument, the handler
 * function for the option will be called with "red" before the "A", "B", "C"
 * arguments are shifted out, then the handler will be called again with "blue"
 * before the "X", "Y", "Z" arguments are shifted out.
 */
class argparser : private util::nocopy {
private:
    // (inner struct) opt
    // An object of this type describes a short or long option to be considered
    // by the parser while processing the argument list.
    struct opt {
        // (var) takes_arg
        // True if the option takes an argument, false otherwise.
        bool takes_arg;

        // (var) deferred
        // If this value is false, the option is processed immediately when
        // `begin' is called. If this value is true, the option is processed at
        // the point when it is encountered while shifting arguments (`>>') or
        // during `pump_eof' calls.
        bool deferred;
        std::function<void(std::string)> f;
    };

    // (var) m_active
    // True if the parser is currently operating on an argument list (between
    // `begin' and `end'). False otherwise.
    bool m_active = false;

    // (var) m_argv
    // The argument list the parser is operating on, if any.
    argv_t m_argv;

    // (var) m_terminated
    // True if the argparser has encountered the `--' terminating argument.
    bool m_terminated;

    // (var) m_opts
    // All of the options defined for this parser. This object is merely a
    // container for the objects themselves, however they are discovered by
    // pointer through the other data structures below.
    std::deque<opt> m_opts;

    // (var) m_shortopts, m_longopts
    // Maps of single characters or strings to the respective opt structures
    // which define how to handle the given option. The opt structures are held
    // in the `m_opts' container above instead of directly here as an option
    // may have multiple aliases.
    std::map<char, opt *> m_shortopts;
    std::map<std::string, opt *> m_longopts;

    // (func) parse
    // Performs the internal argument-list parsing operation.
    void parse(unsigned int &start, bool skip_deferred);

public:
    // (func) begin
    // Begins operation on the given argument list. Any non-deferred options
    // encountered will be immediately processed, their handlers called, and
    // will be removed from the internal copy of the given argument list.
    //
    // Any basic syntax errors in parsing will raise an exception during a call
    // to this function, including errors with deferred options.
    void begin(argv_t argv);

    // (func) end
    // Ends operation on the given argument list. The remaining unparsed part
    // of the list (if any) is processed and any deferred options therein have
    // their handlers called. If any non-option arguments remain, an exception
    // will be thrown.
    void end();

    // (func) add_opt
    // Adds an option (short for `char', long for `std::string') which takes no
    // arguments and is handled by the given function `f'. If an option already
    // exists with the given name, an exception is thrown and no chance occurs.
    void add_opt(
        char shortopt,
        std::function<void()> f,
        bool deferred = false);
    void add_opt(
        std::string longopt,
        std::function<void()> f,
        bool deferred = false);

    // (func) add_opt
    // Adds an option (short for `char', long for `std::string') which takes a
    // single argument and is handled by the given function `f'. If an option
    // already exists with the given name, an exception is thrown and no change
    // occurs.
    void add_opt(
        char shortopt,
        std::function<void(std::string)> f,
        bool deferred = false);
    void add_opt(
        std::string longopt,
        std::function<void(std::string)> f,
        bool deferred = false);

    // (func) alias_opt
    // Adds an option which is exactly the same as another existing option. If
    // the new name (alias) already exists, or if the previous name (target)
    // does not exist, an exception is thrown and no change occurs.
    void alias_opt(char alias, char target);
    void alias_opt(char alias, std::string target);
    void alias_opt(std::string alias, char target);
    void alias_opt(std::string alias, std::string target);

    // (func) pump_eof
    // Processes any deferred options up to the next non-option argument or the
    // end of the list. Returns true if the entire list has been completed, or
    // false otherwise.
    bool pump_eof();

    // (shift op)
    // Processes any deferred options up to the next non-option argument, and
    // then shifts that argument out of the list into the referenced string
    // on the right-hand side of the operator. An exception is thrown if no
    // arguments remain.
    friend argparser &operator >>(argparser &lhs, std::string &rhs);
};

/*
 * core::cmdenv
 *
 * This structure defines the environment in which a subcommand runs, such as
 * the arguments passed to the command, the environment variables, whether or
 * not --help was specified, etc.
 */
struct cmdenv {
    // (var) argv
    // These are the arguments passed to the subcommand. This is similar to the
    // `argv' passed to `main' by the OS, however there are some differences.
    //
    // For example, if the user runs the program like so:
    //
    //   ./drnsf :foo --bar --etc=1234 -- test
    //
    // Then the value of argv will be { "--bar", "--etc=1234", "--", "test" }
    // when passed to the function for the "foo" subcommand.
    argv_t argv;

    // (var) help_requested
    // If this value is true, the subcommand should print information about its
    // usage and available options or parameters, and should then return in a
    // successful manner (EXIT_SUCCESS) without performing any of its usual
    // operations.
    //
    // Normally this value is false, however it is set true if the user is
    // requesting help, such as in `drnsf :help foo-cmd'.
    bool help_requested = false;
};

/*
 * core::main
 *
 * FIXME explain
 */
int main(cmdenv e);

}
}
