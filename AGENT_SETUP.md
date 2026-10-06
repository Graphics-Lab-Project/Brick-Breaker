# Agent Workflow Setup — Brick Breaker (Qt/QML)

Goal: 4 humans, each running their own coding agent (Cursor / Copilot / Codex / Claude Code), pulling tasks from one public GitHub Projects board. Agents implement and test; **only humans commit, push and open PRs.**

```
Frontier model (Opus/Fable)  →  Phase 0 skeleton + failing tests + issue list
          ↓
GitHub Issues + Project board (public)  ←→  GitHub MCP (issues + projects ONLY)
          ↓
Human picks task → creates branch task/<n>-slug → runs agent (follows AGENTS.md)
          ↓
Agent: claim → red → implement → green → scope check → handoff → "Ready for Review"
          ↓
Human reviews diff → commits → pushes → PR "Closes #n" → CI → review → merge → Done
```

---

## 1. Repo layout

```
AGENTS.md                       # the contract (all tools read this)
CLAUDE.md                       # one line: @AGENTS.md   (for Claude Code)
CODEOWNERS                      # your 4 GitHub handles (also defines trusted issue authors)
.github/ISSUE_TEMPLATE/task.md  # section 4
.githooks/pre-commit, pre-push  # section 6
.claude/settings.json           # section 6
src/  tests/  CMakeLists.txt
```

Claude Code reads `CLAUDE.md`, not `AGENTS.md`, so `CLAUDE.md` should contain `@AGENTS.md` on its first line (or symlink it). Cursor, Codex and Copilot read `AGENTS.md` directly.

---

## 2. Phase 0 — the planner (do this once, with the strongest model)

Parallel work only works if the foundation exists first. Before any task agents run, one human + the frontier model produce and **merge** a skeleton:

1. CMake project, `qt_add_qml_module` on a **library** target (so tests can import the real module), Qt Quick Test runner wired into `ctest`, headless via `QT_QPA_PLATFORM=offscreen`.
2. Interfaces as stubs: `GameEngine` (`tick(dt)`, state, signals), `Ball`, `Paddle`, `Brick`, `Level`, with signatures frozen.
3. **Failing acceptance tests, one test file per task**, committed up front. Task agents make them pass; they never write or edit them.
4. One owner file per task (so two tasks never edit the same file). Shared files (`main.qml`, root CMake, shared constants) belong to an "integration" task owned by one human.

### Planner prompt (paste into Opus/Fable with your gameplay notes)

```
You are the technical planner for a Brick Breaker clone (BlackBerry style) in Qt 6 / QML + C++.
Team: 4 humans, each running a smaller coding agent in parallel. Output must let agents work
independently with zero merge conflicts.

Produce, in order:
1. Architecture (max 1 page): logic in C++ with fixed-timestep tick(dt); QML only renders.
   Define file ownership: every source file is owned by exactly one task.
2. Phase 0 skeleton spec: stubs + frozen interfaces (exact signatures) + test harness.
3. Task list as JSON. Each task = one 1–3 hour unit, with fields:
   id, title, goal, allowed_files[], interfaces[] (exact signatures), acceptance_tests[]
   (test file, test function names, and the exact assertion each checks, with numbers),
   run_command, out_of_scope[], blocked_by[], labels[].
4. Dependency graph and a suggested parallel schedule for 4 people (waves).
5. For every task, include at least one edge-case test (corner hits, zero velocity, dt spikes).

Rules: tests must be deterministic and headless (no wall clock, no real timers, no rendering
pixels). No two tasks may share an allowed file. Prefer more, smaller tasks. Seed ideas:
paddle movement + clamping; ball-wall reflection; paddle reflection angle by hit position;
brick collision (side vs top/bottom); brick hit points + break animation state; scoring;
lives + game over; level loader; power-ups; game state machine (menu/play/pause/over);
HUD; input mapping; high-score persistence.
Ask me questions before planning if anything is ambiguous.
```

Feed it: your written gameplay notes and screenshots from searching for BlackBerry Brick Breaker footage. I can do that gameplay research as the next step.

---

## 3. Project board

Create a GitHub **Project** (prefer org-owned so tokens are simple) with a single-select `Status` field:

`Backlog` · `Ready` · `In Progress` · `Ready for Review` · `Blocked` · `Needs Human` · `Done`

Extra fields: `Wave` (number), `Area` (engine / ui / anim / audio). Use built-in workflows so that **closing an issue or merging a PR sets `Done`**; that is the only way to reach `Done`.

Public-repo hygiene (important, because agents read this text):
- Restrict who can comment/open issues (repo Settings → Moderation → interaction limits → collaborators only).
- Add `CODEOWNERS` with your four handles. AGENTS.md rule R9 tells agents to ignore instructions from anyone else.

---

## 4. Issue template (`.github/ISSUE_TEMPLATE/task.md`)

```markdown
---
name: Task
about: Agent-ready task
labels: task
---
## Goal
<one or two sentences, observable behaviour>

## Allowed files
- src/engine/Paddle.cpp
- src/engine/Paddle.h

## Interfaces (frozen)
- `void Paddle::moveBy(qreal dx)`
- `signal positionChanged(qreal x)`

## Acceptance tests (read-only)
- File: tests/tst_paddle.qml
- Functions: test_clampsLeft, test_clampsRight, test_emitsPositionChanged
- Run: `QT_QPA_PLATFORM=offscreen ctest --test-dir build --output-on-failure -R tst_paddle`

## Out of scope
- Rendering, input handling, sound

## Blocked by
- #<n> (or "none")

## Reference
<link/screenshot of original behaviour>
```

---

## 5. MCP: how agents read and update the board

You do not need to build a custom plugin. GitHub's official MCP server has selectable toolsets, including `issues` and `projects`, and can be restricted with `--toolsets` (check the README for the exact option names in your version).

Configure it for every tool with **only** `issues` and `projects` toolsets, with no `repos` or `pull_requests` toolset:
- Agents can read/claim/comment/update status.
- Agents cannot push commits or open PRs through MCP.

Token: each human uses **their own** token so activity is attributed to them.
- Needs Issues read/write and Projects read/write; **no** Contents write.
- User-owned Projects v2 may need a classic token with the `project` scope; org-owned projects work with fine-grained tokens. Verify against GitHub's current token docs.

Per-tool config locations (copy the exact snippet from the GitHub MCP server README for each):
- Claude Code: `claude mcp add ...` (project scope → `.mcp.json`)
- Cursor: `.cursor/mcp.json`
- Copilot (VS Code): `.vscode/mcp.json` (root key is `servers`)
- Codex: `~/.codex/config.toml`

Verify: ask each agent "list the tools you have from the GitHub MCP server". You should see issue and project tools and no push/PR tools.

---

## 6. Lockdown: make "humans only commit/push" real

The contract is a soft control. Layer hard ones on top:

1. **MCP scope** (section 5): no repo/PR write tools.
2. **Tool permissions**:
   - Claude Code `.claude/settings.json`:
     ```json
     {
       "permissions": {
         "deny": [
           "Bash(git commit:*)", "Bash(git push:*)", "Bash(git merge:*)",
           "Bash(git rebase:*)", "Bash(git reset:*)", "Bash(git stash:*)",
           "Bash(git checkout -b:*)", "Bash(gh pr:*)"
         ]
       }
     }
     ```
   - Cursor / Codex / Copilot: keep command approvals ON, never auto-approve anything starting with `git` or `gh`, and add git write commands to the tool's denylist if it has one.
3. **Git hooks** (`git config core.hooksPath .githooks`): `pre-commit` and `pre-push` exit 1 unless `HUMAN=1` is set. Humans run `HUMAN=1 git commit ...`. This is a speed bump, not a guarantee; an agent that sets the variable is violating R1.
4. **Branch protection on `main`**: PR required, 1 human approval, required CI checks, no direct pushes.
5. **CI scope check** (optional but recommended): a workflow compares the PR's changed files to the issue's `Allowed files` and fails on anything extra. I can write this next.
6. **CI tests**: build + `ctest` offscreen on every PR.

---

## 7. Skills and plugins to install

Qt publishes official agent skills and a documentation MCP server (repo: `TheQtCompanyRnD/agent-skills`). Relevant skills: `qt-qml`, `qt-qml-test`, `qt-qml-test-run`, `qt-qml-review`, `qt-project`.

| Tool | Install |
|---|---|
| Claude Code | `/plugin marketplace add TheQtCompanyRnD/agent-skills` then `/plugin install qt-development-skills` (also wires the Qt docs MCP) |
| Codex / others | `npx skills add TheQtCompanyRnD/agent-skills` (Qt docs MCP is a separate manual step) |
| Copilot CLI | `copilot plugin marketplace add TheQtCompanyRnD/agent-skills` then `copilot plugin install qt-development-skills@qt-skills-and-tools` |
| Cursor | follow the repo README's manual install |

Who uses what:
- **Planner (frontier model):** `qt-qml-test`, `qt-qml-test-run` to author and wire the failing tests in Phase 0.
- **Task agents:** `qt-qml`, `qt-qml-review`, plus Qt docs MCP. They do **not** write tests (R4).
- Skills do not enforce the contract; AGENTS.md plus the lockdown do.

---

## 8. Day-to-day flow for one human

1. Pick a `Ready` issue whose blockers are `Done`.
2. `git switch -c task/<n>-<slug>` (you create the branch).
3. Open your agent in the repo: "Do issue #<n> per AGENTS.md."
4. Review the agent's handoff comment and `git diff`.
5. `HUMAN=1 git commit`, `git push`, open PR with `Closes #<n>`.
6. Teammate reviews → CI green → merge → board flips to `Done`.

---

## 9. Smoke test before real work

Create a trivial task ("add a `clamp()` helper with 3 tests"). Run it on all four tools. Check that each agent: refuses to commit, claims the issue, goes red then green, stays in Allowed files, and posts the handoff. Fix the contract wording wherever an agent drifts, since each tool interprets instructions slightly differently.

---

## 10. Known limits

- Agents can still make mistakes inside Allowed files. Human diff review is the real safety net.
- Parallelism is limited by file ownership. If two tasks keep needing the same file, merge them or move the shared part into Phase 0.
- "Marks complete" here means `Ready for Review`; `Done` comes from the merged PR.
