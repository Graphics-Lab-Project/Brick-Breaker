#!/usr/bin/env python3
"""
Commit one finished task locally (never pushes).

    python3 scripts/commit_task.py E4

1. Refuses if the task's test does not pass in ./build (run task_status.py first).
2. Refuses if any frozen path is modified: tests/, CMakeLists.txt, src/engine/*.h,
   src/engine/GameEngine_loop.cpp, src/app/, qml/Theme.qml, tests/qml/MockEngine.qml.
3. Stages ONLY this task's Allowed files that changed, and commits them.
Changes that belong to other tasks are left alone (they are still being worked on).
"""
import fnmatch, os, re, subprocess, sys
sys.dont_write_bytecode = True
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from task_status import ROOT, BUILD, load_tasks  # noqa: E402

FROZEN = ["tests/*", "CMakeLists.txt", "src/engine/*.h", "src/engine/GameEngine_loop.cpp",
          "src/app/*", "qml/Theme.qml"]


def git(*a, check=True):
    r = subprocess.run(["git", "-C", str(ROOT), *a], capture_output=True, text=True)
    if check and r.returncode:
        sys.exit(f"git {' '.join(a)} failed: {r.stderr.strip()}")
    return r.stdout


def changed_files():
    out = git("status", "--porcelain", "--untracked-files=all")
    files = []
    for line in out.splitlines():
        path = line[3:].split(" -> ")[-1].strip().strip('"')
        files.append(path)
    return files


def main():
    if len(sys.argv) != 2:
        sys.exit(__doc__)
    key = sys.argv[1].upper()
    tasks = load_tasks()
    if key not in tasks:
        sys.exit(f"unknown task {key}")
    t = tasks[key]

    env = dict(os.environ, QT_QPA_PLATFORM="offscreen")
    r = subprocess.run(["ctest", "--test-dir", str(BUILD), "-R", f"^{t['test']}$"],
                       capture_output=True, text=True, env=env)
    if r.returncode or "100% tests passed" not in r.stdout:
        sys.exit(f"REFUSED: {t['test']} does not pass in ./build. Rebuild and re-run task_status.py.")

    changed = changed_files()
    frozen = [f for f in changed if any(fnmatch.fnmatch(f, p) for p in FROZEN)]
    if frozen:
        sys.exit("REFUSED: frozen files are modified (restore them with `git restore <file>`):\n  "
                 + "\n  ".join(frozen))

    owned = {f for tk in tasks.values() for f in tk["files"]}
    stray = [f for f in changed if f not in owned and f != "tasks/LEDGER.md"]
    mine = [f for f in changed if f in t["files"]]
    if not mine:
        print(f"nothing to commit for {key} (already committed?)")
        return
    git("add", "--", *mine)
    subject = re.sub(r"^(Engine|UI|Integration): ", "", t["title"])
    msg = f"feat({t['area']}): {subject} [{key}]\n\nAcceptance test {t['test']} passes."
    git("commit", "-m", msg, "--", *mine)
    print(f"committed {key}: " + ", ".join(mine))
    print(git("log", "--oneline", "-1").strip())
    if stray:
        print("WARNING stray files (owned by no task; inspect, then delete or restore):\n  " + "\n  ".join(stray))


if __name__ == "__main__":
    main()
