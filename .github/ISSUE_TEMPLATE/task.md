---
name: Task
about: Agent-ready task
labels: task
---
## Goal
<one or two sentences, observable behaviour>

## Allowed files
- src/engine/Example.cpp
- ci/green/tst_example

## Interfaces (frozen)
- See the header / stub file. Do not change signatures, properties, signals or objectNames.

## Acceptance tests (read-only)
- File: tests/.../tst_example
- Run: `ctest --test-dir build --output-on-failure -R "^tst_example$"`

## Spec notes
<rules the tests check>

## Out of scope
<what not to touch>

## Blocked by
- none

## Reference
- docs/DESIGN_HANDOFF.md, docs/PLAN.md
