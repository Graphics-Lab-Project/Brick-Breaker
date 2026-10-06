# Example Flow — one task from board to merge

Worked example: **Issue #7 — Paddle clamping** (Alice runs Cursor, Bob reviews). Every task follows this same loop.

Legend: 🧑 = human does it · 🤖 = agent does it · ⚙️ = automation does it

---

## The big picture

```
🧑 pick task → 🧑 make branch → 🤖 claim, red, implement, green, handoff
→ 🧑 review diff → 🧑 commit + push → 🧑 open PR → ⚙️ CI + scope check
→ 🧑 teammate approves → 🧑 squash merge → ⚙️ issue closes, card = Done
```

---

## Step 1 — 🧑 Pick a task

On the Project board, choose a card in **Ready** whose "Blocked by" issues are all **Done**. Open issue #7 and read it. It looks like this:

```markdown
## Goal
Paddle position is clamped to the playfield and emits positionChanged.

## Allowed files
- src/engine/Paddle.cpp
- src/engine/Paddle.h

## Interfaces (frozen)
- void Paddle::moveBy(qreal dx)
- signal positionChanged(qreal x)

## Acceptance tests (read-only)
- File: tests/tst_paddle.qml
- Functions: test_clampsLeft, test_clampsRight, test_emitsPositionChanged
- Run: ctest --test-dir build --output-on-failure -R tst_paddle

## Out of scope
- Rendering, input handling, sound

## Blocked by
- none
```

Tell the team in chat: "Taking #7." (The agent will also claim it on the board, but humans should avoid duplicates.)

---

## Step 2 — 🧑 Create your branch (the agent will not)

```bash
git switch main
git pull
git switch -c task/7-paddle-clamp
```

Make sure the project builds before you start:
```bash
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="<your Qt path>"
cmake --build build
```

---

## Step 3 — 🧑 Start your agent

Open the repo in Cursor / Copilot / Codex / Claude Code. Prompt:

```
Work on GitHub issue #7 in this repo. Follow AGENTS.md exactly.
Do not run any git write commands. Ask me if anything is unclear.
```

---

## Step 4 — 🤖 What the agent should do (watch for these)

1. Reads `AGENTS.md`, then reads issue #7 through the GitHub MCP tools.
2. **Preflight:** status is Ready, no blockers, branch is `task/7-paddle-clamp`, tests exist.
3. **Claims:** the card moves to **In Progress** and an issue comment says "Claimed by <tool> for @alice".
4. **Red:** runs the paddle tests and shows them failing for the right reason.
5. **Implements** only in `Paddle.cpp` / `Paddle.h`.
6. **Green:** paddle tests pass, then the full suite passes.
7. **Scope check:** lists changed files; all are in Allowed files.
8. **Handoff:** posts the handoff comment on the issue, moves the card to **Ready for Review**, and prints a suggested commit message.

Red flags (stop it and remind it of the contract):
- It runs or proposes `git commit`, `git push`, or `gh pr create`.
- It edits `tests/tst_paddle.qml` or any file outside Allowed files.
- It says "all tests pass" without showing a command and output.
- It "fixes" something unrelated "while it's there".

---

## Step 5 — 🧑 Review the diff yourself

The agent's word is not enough. You are the quality gate.

```bash
git status
git diff --stat
git diff
git diff --stat -- tests/      # must print nothing
```

Checklist:
- [ ] Only `Paddle.cpp` and `Paddle.h` changed
- [ ] Nothing in `tests/` changed
- [ ] No `QSKIP`, commented-out assertions, or hard-coded values that match the test numbers
- [ ] Code reads sensibly; you could explain it to Bob
- [ ] Re-run locally: `ctest --test-dir build --output-on-failure`

If a file outside scope changed:
```bash
git restore path/to/stray_file
```

---

## Step 6 — 🧑 Commit and push (only you)

Add files by explicit path, never `git add .`:
```bash
git add src/engine/Paddle.cpp src/engine/Paddle.h
git commit -m "feat(engine): clamp paddle to playfield (#7)"
git push -u origin task/7-paddle-clamp
```

If you installed the optional `HUMAN=1` hook, commit with:
- bash/zsh: `HUMAN=1 git commit -m "..."`
- PowerShell: `$env:HUMAN="1"; git commit -m "..."`

---

## Step 7 — 🧑 Open the PR

With GitHub CLI:
```bash
gh pr create --title "feat(engine): clamp paddle to playfield" --body "Closes #7"
```
or click the link Git prints after the push. Fill in the PR template. **The description must contain `Closes #7`**, or scope-check fails.

---

## Step 8 — ⚙️ Automation runs

On the PR page, under checks:
- **build-test**: builds on Linux and runs all tests headless.
- **scope-check**: confirms the PR only touches files listed in issue #7.

Both must be green.

If **build-test** is red and it was green locally, look for: file-name case differences, a missing dependency, or a test that needs a real window.

To fix: paste the CI error into your agent (rework on the same issue):

```
CI failed on my PR for issue #7. Here is the log: <paste>.
Fix it following AGENTS.md. Same scope rules apply.
```
The agent cannot see PRs or CI, so you paste what it needs. Then repeat steps 5–6 (new commit on the same branch, same push).

---

## Step 9 — 🧑 Teammate review

Bob opens the PR → **Files changed** → reads the diff → leaves comments if needed → **Review changes → Approve**.
Authors resolve every conversation. Nobody approves their own PR.

---

## Step 10 — 🧑 Merge

When checks are green and there is 1 approval, Bob (or Alice) clicks **Squash and merge**.

⚙️ Then automatically:
- the branch is deleted,
- issue #7 closes (because of `Closes #7`),
- its board card moves to **Done**.

---

## Step 11 — 🧑 Clean up and loop

```bash
git switch main
git pull
git branch -d task/7-paddle-clamp
```
Pick the next **Ready** card. Tasks that were blocked by #7 should now be moved to **Ready** (by the integrator, or automatically if you add a rule).

---

## What if…?

| Situation | What to do |
|---|---|
| **Agent asks a question** | Answer in chat. If it is a spec problem, also comment on the issue so the answer is public. |
| **Agent says a test is wrong** | Do not let it edit the test. Comment on the issue and tag the planner. The planner fixes the test in a separate PR (label `skip-scope`). Everyone then runs `git switch main && git pull && git switch <branch> && git merge main`. |
| **Agent wants a change in a shared file** | It should stop and write a comment. The integrator makes a new task or does it. |
| **Your PR conflicts with `main`** | `git fetch origin && git merge origin/main`, fix the conflict markers, build + test again, commit, push. Ask the integrator if it is more than a trivial conflict. |
| **Blocked by an unmerged task** | Pick a different Ready task. Do not stub or re-implement the dependency. |
| **Agent tried to commit anyway** | Reject it. Tighten the tool's deny list (see AGENT_SETUP.md section 6). Reset only with your own commands, not the agent's. |
| **Two people grabbed the same issue** | The first person assigned wins. The other releases it and picks another. |
| **Agent is stuck in a loop** | After 3 failed attempts it should stop and ask. If it does not, stop it and restart with a clearer prompt. |

---

## Suggested team rhythm

- **Start of work session (2 min, chat):** "Taking #7", "Taking #9".
- **Every PR:** a teammate reviews within the day.
- **Waves:** finish all Wave 1 tasks before the integrator unlocks Wave 2.
- **Before demo/submission:** freeze `main`, run full suite on all three OSes, tag a release (the integrator does this).

## Mini Git cheat sheet

| Goal | Command |
|---|---|
| Where am I? | `git status`, `git branch --show-current` |
| Latest main | `git switch main && git pull` |
| New task branch | `git switch -c task/<n>-<slug>` |
| See my changes | `git diff` |
| Undo edits in one file | `git restore <file>` |
| Stage specific files | `git add <file> <file>` |
| Commit | `git commit -m "type(scope): message (#n)"` |
| Push first time | `git push -u origin <branch>` |
| Bring in latest main | `git fetch origin && git merge origin/main` |
