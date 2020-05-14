# Group 3

## Commit Message Format
`[#<IssueNumber>] <Commit Message>` where you replace \<IssueNumber> with the number of the issue you're working on and \<Commit Message> with your Commit Message 

## Building

Build the normal parser using `make` or `make parser`. The parser is written to `bin/TestCPP`.

### Debug Builds

A debug parser can be build using `make debug`. It is written to `debug/TestCPP`.

### Tests

Tests are files stored in the `tests/` directory. All tests can be executed automatically using
`make test`, this will always use a normal (non-debug) parser. Tests are executed until the first
one fails, at which point execution is stopped.

Individual tests can be easily executed using the `./run-test` command. This will always use
a debug parser if one is present and otherwise use the normal parser.

Executing just `./run-tests` will run all tests, following the same rules as `make test` (except
for using a debug parser if present.) It is also possible to pass a list of test names as arguments,
which are then the only ones executed (e.g. `./run-tests 1-hello.cc 5-palin.cc`).