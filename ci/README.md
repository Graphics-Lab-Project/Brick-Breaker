# ci/green

One empty-ish file per test that must stay green, named exactly like the ctest test
(e.g. `ci/green/tst_collision`). CI runs every listed test and fails the PR if any fails.

Each task adds **its own** marker in the same PR that makes its test pass (the marker path
is in the issue's Allowed files). One file per test means no merge conflicts.
The file content is just the issue number, e.g. `#7`.
