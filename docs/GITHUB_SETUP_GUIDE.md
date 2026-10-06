# GitHub Setup Guide — Brick Breaker (Qt/QML), 4 people, free GitHub Organization

Read this top to bottom once. Sections 1–8 are one-time setup. Section 9 is a checklist so you know it all works.

Companion files (keep in the **repo root**): `AGENTS.md` (agent contract, do not edit casually) and `AGENT_SETUP.md` (design notes). `EXAMPLE_FLOW.md` shows one task end to end.

---

## 0. Message to paste in your group chat

> 1. Make a GitHub account (turn on 2FA) and send me your username.
> 2. Install Git, then run: `git config --global user.name "Your Name"` and `git config --global user.email "your-github-email"`.
> 3. Install Qt **6.8 LTS** (Desktop) + CMake + Ninja with the Qt Online Installer (details in the guide, section 3).
> 4. Accept my invite to the GitHub org, then clone the repo.
> 5. Read `AGENTS.md` in the repo root. **Your AI agent never runs git commit/push. You do.**
> 6. Never work directly on `main`. One branch per task.

---

## 1. Roles (4 people)

| Role | Who | Does |
|---|---|---|
| Org owner + integrator | A | Creates org/repo, owns CI, `main.qml`, root `CMakeLists.txt` |
| Second owner | B | Backup admin (so you are not locked out if A disappears) |
| Planner | A or B | Runs the frontier model (Opus/Fable) to produce Phase 0 + issues |
| Developers | all 4 | Pick tasks, run agents, review each other's PRs |

Rule of thumb: nobody merges their own PR. A teammate reviews every PR.

---

## 2. One-time setup (org owner)

### 2.1 Create the organization
1. GitHub → **+** (top right) → **New organization** → choose **Free**.
2. Name it, e.g. `jgu-cg-brickbreaker`. Add a contact email.
3. Invite teammates: Org → **People** → **Invite member**. Make **one other person** an **Owner**, the rest **Members**.
4. Org → **Settings** → **Member privileges** → **Base permissions: Read**.
5. Org → **Teams** → **New team** `devs` → add all 4 people. (You will give this team **Write** on the repo.)

### 2.2 Create the repository
1. Org → **New repository**: name `brick-breaker`, **Public**, add README, add **.gitignore → C++**, no license needed for a lab.
2. Repo → **Settings** → **Collaborators and teams** → **Add teams** → `devs` → role **Write**.

> **Why Public matters:** on the free plan, branch protection and rulesets only work on public repos (per GitHub's docs, protected branches are available in public repositories owned by a GitHub Free organization). A private repo would silently lose the protections in section 5.

### 2.3 Repo settings (Settings → General)
- **Pull Requests:** allow **Squash merging only** (untick merge commit and rebase). Default commit message: **Pull request title and description**.
- Tick **Automatically delete head branches**.
- **Issues:** enabled. **Projects:** enabled.

### 2.4 Actions settings (Settings → Actions → General)
- Allow actions from GitHub and verified marketplace creators.
- Workflow permissions: **Read repository contents** (our workflows declare their own permissions).

### 2.5 Labels (Issues → Labels)
Create: `task`, `needs-human`, `skip-scope`, `area:engine`, `area:ui`, `area:anim`, `area:audio`.

### 2.6 Lock down who can comment (public repo hygiene)
Agents read issue text, so strangers must not inject instructions.
Repo → **Settings** → **Moderation options** → **Interaction limits** → limit to **repository collaborators**, with the longest duration offered. These expire, so renew before the deadline.

---

## 3. Every teammate: machine setup

| Item | Windows | macOS | Linux |
|---|---|---|---|
| Git | git-scm.com (Git for Windows) | `xcode-select --install` | `sudo apt install git` |
| Compiler | Visual Studio 2022 Build Tools (C++ workload) *or* MinGW from Qt installer | Xcode Command Line Tools | `sudo apt install build-essential` |
| Qt | Qt Online Installer → **Qt 6.8 LTS** → Desktop (MSVC 2022 64-bit or MinGW) | Qt 6.8 → macOS | Qt 6.8 → Desktop gcc 64-bit |
| CMake + Ninja | Qt installer → *Developer and Designer Tools* → CMake, Ninja | same | same (or apt) |
| GitHub CLI (optional, easier PRs) | `winget install GitHub.cli` | `brew install gh` | see cli.github.com |

Use the **same Qt minor version as CI (6.8)** so "works on my machine" means "works in CI".

Sign in and clone:
```bash
gh auth login            # choose GitHub.com, HTTPS, browser login
gh repo clone <org>/brick-breaker
cd brick-breaker
```

Cross-platform traps (Windows/macOS/Linux mix):
- **Case sensitivity.** Linux CI is case-sensitive; Windows/macOS usually are not. `import "paddle.qml"` vs `Paddle.qml` works locally and fails in CI. Match file-name case exactly.
- **Line endings.** Use the `.gitattributes` below.
- **Headless tests.** Never ask teammates to set `QT_QPA_PLATFORM` by hand (Windows shell syntax differs). The Phase 0 CMake should set `ENVIRONMENT "QT_QPA_PLATFORM=offscreen"` on each test via `set_tests_properties`.
- **No absolute paths** in CMake. Let each machine pass `-DCMAKE_PREFIX_PATH=<its Qt>`.

---

## 4. Files to add to the repo (owner, one initial commit to `main`)

Root layout:
```
AGENTS.md
AGENT_SETUP.md
CLAUDE.md
.gitattributes
.github/CODEOWNERS
.github/pull_request_template.md
.github/ISSUE_TEMPLATE/task.md          (from AGENT_SETUP.md section 4)
.github/workflows/ci.yml
.github/workflows/scope-check.yml
```

**CLAUDE.md** (only matters if someone uses Claude Code):
```
@AGENTS.md
```

**.gitattributes**
```
* text=auto eol=lf
*.png binary
*.jpg binary
*.wav binary
*.ogg binary
```

**.github/CODEOWNERS** (also the list of people agents may trust, see AGENTS.md R9)
```
* @handle-a @handle-b @handle-c @handle-d
```

**.github/pull_request_template.md**
```markdown
Closes #<issue number>

## What changed
<1–3 lines>

## Evidence
- Agent used: <tool>
- Test command + result: `<paste real output summary>`
- Local build on: <Windows/macOS/Linux>

## Human checklist (author)
- [ ] I read the whole diff
- [ ] No test files changed
- [ ] Only files from the issue's "Allowed files" changed
- [ ] The agent did not commit or push; I did
```

**.github/workflows/ci.yml** (build + headless tests on every PR)
```yaml
name: CI

on:
  pull_request:
    branches: [main]
  push:
    branches: [main]

concurrency:
  group: ci-${{ github.ref }}
  cancel-in-progress: true

permissions:
  contents: read

jobs:
  build-test:
    name: build-test
    runs-on: ubuntu-latest
    env:
      QT_QPA_PLATFORM: offscreen
    steps:
      - uses: actions/checkout@v4

      # Until Phase 0 adds CMakeLists.txt this job passes as a no-op.
      - name: Detect project
        id: proj
        run: |
          if [ -f CMakeLists.txt ]; then echo "has=true" >> "$GITHUB_OUTPUT"; else echo "has=false" >> "$GITHUB_OUTPUT"; fi

      - name: Install Qt
        if: steps.proj.outputs.has == 'true'
        uses: jurplel/install-qt-action@v4
        with:
          version: '6.8.*'
          cache: true
          # add modules here only if the project needs them, e.g. modules: 'qtmultimedia'

      - name: Install build tools and runtime libs
        if: steps.proj.outputs.has == 'true'
        run: |
          sudo apt-get update
          sudo apt-get install -y ninja-build libgl1-mesa-dev libegl1 libxkbcommon-x11-0 libfontconfig1

      - name: Configure
        if: steps.proj.outputs.has == 'true'
        run: cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug

      - name: Build
        if: steps.proj.outputs.has == 'true'
        run: cmake --build build

      - name: Test (headless)
        if: steps.proj.outputs.has == 'true'
        run: ctest --test-dir build --output-on-failure
```
Qt is installed by the `jurplel/install-qt-action` community action, which adds the Qt bin directory to PATH so CMake can find Qt. If the build fails with a missing `.so`, add that package to the apt line.

**.github/workflows/scope-check.yml** (fails the PR if it touches files the issue did not allow)
```yaml
name: Scope check

on:
  pull_request:
    branches: [main]
    types: [opened, synchronize, reopened, edited, labeled, unlabeled]

permissions:
  contents: read
  issues: read
  pull-requests: read

jobs:
  scope-check:
    name: scope-check
    runs-on: ubuntu-latest
    steps:
      - uses: actions/github-script@v7
        with:
          script: |
            const pr = context.payload.pull_request;
            const labels = pr.labels.map(l => l.name);
            if (labels.includes('skip-scope')) {
              core.notice('skip-scope label present: scope check skipped.');
              return;
            }

            const m = (pr.body || '').match(/(?:closes|fixes|resolves)\s+#(\d+)/i);
            if (!m) {
              core.setFailed('PR description must contain "Closes #<issue number>".');
              return;
            }

            const { data: issue } = await github.rest.issues.get({
              owner: context.repo.owner,
              repo: context.repo.repo,
              issue_number: Number(m[1]),
            });

            const body = issue.body || '';
            const section = body.match(/##\s*Allowed files\s*\n([\s\S]*?)(?:\n##\s|$)/i);
            if (!section) {
              core.setFailed(`Issue #${m[1]} has no "## Allowed files" section.`);
              return;
            }

            const allowed = section[1]
              .split('\n')
              .map(l => l.trim())
              .filter(l => l.startsWith('-'))
              .map(l => l.replace(/^-\s*/, '').replace(/`/g, '').trim())
              .filter(Boolean);

            if (allowed.length === 0) {
              core.setFailed(`Issue #${m[1]} lists no allowed files.`);
              return;
            }

            const isAllowed = (file) => allowed.some(a => {
              if (a.endsWith('/**')) return file.startsWith(a.slice(0, -2));
              if (a.endsWith('/')) return file.startsWith(a);
              return file === a;
            });

            const files = await github.paginate(github.rest.pulls.listFiles, {
              owner: context.repo.owner,
              repo: context.repo.repo,
              pull_number: pr.number,
              per_page: 100,
            });

            const outside = files.map(f => f.filename).filter(f => !isAllowed(f));
            if (outside.length > 0) {
              core.setFailed(
                `Files outside issue #${m[1]} "Allowed files":\n` + outside.map(f => ` - ${f}`).join('\n')
              );
            } else {
              core.notice(`OK: ${files.length} changed file(s), all within issue #${m[1]} scope.`);
            }
```
It reads the issue named in `Closes #n`, parses the `## Allowed files` list, and compares it to the PR's changed files. `dir/**` and `dir/` match a whole folder. Use the `skip-scope` label only for non-task PRs (docs, Phase 0, CI), and reviewers must reject its misuse.

Push these to `main`:
```bash
git add .
git commit -m "chore: add agent contract, CI, templates"
git push origin main
```
(Allowed now because no ruleset exists yet. This is the last direct push to `main`.)

---

## 5. Protect `main` (after CI has run once)

A required check only shows up in the picker after it has run, and per GitHub's docs it must have completed successfully in the repo within the past seven days. So:

1. Open the Actions tab and confirm **CI / build-test** ran on your push. (Until Phase 0 adds a `CMakeLists.txt`, it passes as a harmless no-op, which is enough to register the check.)
2. Make a tiny PR (e.g. fill in the real handles in CODEOWNERS), add label `skip-scope`, wait for **scope-check** to run, and merge it now (no ruleset exists yet).
3. Repo → **Settings** → **Rules** → **Rulesets** → **New branch ruleset**:
   - Name `protect-main`, Enforcement **Active**, Target **Default branch**.
   - Tick: **Restrict deletions**, **Block force pushes**.
   - **Require a pull request before merging** → required approvals **1**, **Dismiss stale approvals**, **Require conversation resolution**.
   - **Require status checks to pass** → add `build-test` and `scope-check`; tick **Require branches to be up to date**.
   - **Bypass list:** empty.
4. GitHub's docs say rulesets are available on public repos with GitHub Free.

Because CI is a no-op before Phase 0, requiring `build-test` from day one is safe.

---

## 6. Project board (GitHub Projects)

1. Org → **Projects** → **New project** → **Board**. Name `Brick Breaker`.
2. Edit the **Status** field to exactly: `Backlog`, `Ready`, `In Progress`, `Ready for Review`, `Blocked`, `Needs Human`, `Done`.
3. Add fields: `Wave` (number), `Area` (single select).
4. Project **⋯** → **Workflows** → enable:
   - **Auto-add to project**: repo `brick-breaker`, filter `is:issue label:task` (then set Status to Backlog).
   - **Item closed** → Status `Done`.
   - **Pull request merged** → Status `Done`.
5. Link the repo to the project (Project **Settings** → add repository) and set the project **Public**.

Agents move cards to `In Progress` and `Ready for Review`. Only a merged PR reaches `Done`.

---

## 7. Connect agents to the board (MCP)

Each person does this once, with **their own** token:

1. GitHub → profile → **Settings** → **Developer settings** → **Personal access tokens** → **Fine-grained tokens** → **Generate**.
2. Resource owner: **the org**. Repository access: **only `brick-breaker`**.
3. Permissions: **Issues: read/write**, **Projects: read/write**, **Metadata: read**. **No Contents, no Pull requests.**
4. The org owner may need to approve the token (Org → Settings → Personal access tokens). Project permissions for org projects may be listed under organization permissions. Check GitHub's current token docs if you do not see them.
5. Add GitHub's MCP server to your tool with toolsets `issues,projects` only. Exact snippets per tool: see `AGENT_SETUP.md` section 5 and the GitHub MCP server README.
6. Test: ask the agent "list the GitHub MCP tools you have". You should see issue and project tools and **no** push or PR tools.

Never paste the token into chat or commit it.

Qt skills (optional, recommended): see `AGENT_SETUP.md` section 7.

---

## 8. Phase 0 (before anyone runs task agents)

One person with the frontier model produces and **merges** (via PR, label `skip-scope`):
1. CMake project + Qt Quick Test runner wired into `ctest`, headless.
2. Stub classes with frozen interfaces.
3. Failing test files, one per task.
4. The issue list (use the planner prompt in `AGENT_SETUP.md`), created as GitHub issues with label `task` and Status `Ready` for wave 1.

From this PR onward, `build-test` runs for real (build + headless tests) and gates every merge.

---

## 9. Verification checklist

- [ ] All 4 people can `git clone` and build locally
- [ ] A direct `git push origin main` is **rejected**
- [ ] A PR cannot merge without 1 approval and green checks
- [ ] A PR with no `Closes #n` fails **scope-check**
- [ ] A PR that edits a file not in Allowed files fails **scope-check**
- [ ] Closing an issue moves its card to **Done**
- [ ] Each agent can read an issue and change its Status via MCP
- [ ] Each agent **refuses** `git commit` (deny list works)
- [ ] An outsider account cannot comment on issues

---

## 10. Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| CI can't find Qt | `install-qt-action` step failed; check its log and the Qt version string |
| CI: missing `libXYZ.so` | add the package to the `apt-get install` line |
| Passes locally, fails in CI | file-name case, missing dependency, or test depends on a window; run `QT_QPA_PLATFORM=offscreen` locally |
| Required check never appears | it must have run once in the last 7 days; open a PR to trigger it |
| scope-check: "no Allowed files" | issue body lacks the `## Allowed files` heading |
| `git push` rejected on `main` | expected; push your task branch and open a PR |
| Agent can't see the Project | token lacks Projects permission, or the org has not approved it |
| Ruleset options missing | repo is private on a free plan; make it public |
