#!/usr/bin/env python3
"""
Which tasks are done? A task is DONE exactly when its acceptance test passes.

    python3 scripts/task_status.py            # build ./build, run every test, print the table
    python3 scripts/task_status.py --no-build # reuse the last build
    python3 scripts/task_status.py --json     # machine-readable

READY = not done and every task in "Depends on" is done.
Exit code 0 always (the table is the result); 3 if the build itself failed.
"""
import json, os, re, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"


def load_tasks():
    tasks = {}
    for f in sorted((ROOT / "tasks").glob("*.md")):
        text = f.read_text()
        m = re.match(r"# \[(\w+)\] (.+)", text)
        if not m:
            continue
        key, title = m.groups()
        row = re.search(r"\|\s*(haiku|sonnet|opus)\s*\|\s*(\d+)\s*\|\s*(\w+)\s*\|\s*([^|]+)\|", text)
        deps = [d.strip() for d in row.group(4).split(",") if d.strip() and d.strip() != "none"]
        test = re.search(r"ctest name: `(tst_\w+)`", text).group(1)
        allowed = re.search(r"## Allowed files[^\n]*\n(.*?)\n\n", text, re.S).group(1)
        files = [l[2:].strip() for l in allowed.splitlines() if l.startswith("- ")]
        tasks[key] = dict(key=key, title=title, model=row.group(1), wave=int(row.group(2)),
                          area=row.group(3), deps=deps, test=test, files=files)
    return tasks


def build():
    env = dict(os.environ, QT_QPA_PLATFORM="offscreen")
    if not (BUILD / "CMakeCache.txt").exists():
        r = subprocess.run(["cmake", "-S", str(ROOT), "-B", str(BUILD), "-G", "Ninja"],
                           capture_output=True, text=True, env=env)
        if r.returncode:
            print(r.stdout[-3000:], r.stderr[-3000:]); sys.exit(3)
    r = subprocess.run(["cmake", "--build", str(BUILD)], capture_output=True, text=True, env=env)
    if r.returncode:
        print("BUILD FAILED (fix before anything else):")
        print((r.stdout + r.stderr)[-4000:])
        sys.exit(3)


def run_tests():
    env = dict(os.environ, QT_QPA_PLATFORM="offscreen")
    r = subprocess.run(["ctest", "--test-dir", str(BUILD), "-j", str(os.cpu_count() or 4)],
                       capture_output=True, text=True, env=env)
    result = {}
    for line in r.stdout.splitlines():
        m = re.search(r"Test\s+#\d+:\s+(\S+)\s+\.+\**\s*(Passed|\*\*\*Failed|Failed|\*\*\*Timeout|Timeout|Not Run|\*\*\*Exception.*)", line)
        if m:
            result[m.group(1)] = m.group(2) == "Passed"
    return result


def main():
    args = sys.argv[1:]
    tasks = load_tasks()
    if "--no-build" not in args:
        build()
    passed = run_tests()
    done = {k for k, t in tasks.items() if passed.get(t["test"])}
    for t in tasks.values():
        t["done"] = t["key"] in done
        t["ready"] = not t["done"] and all(d in done for d in t["deps"])
    sanity = {n: passed.get(n, False) for n in ("tst_phase0_sanity", "tst_qml_sanity")}
    if "--json" in args:
        print(json.dumps({"tasks": list(tasks.values()), "sanity": sanity}, indent=1))
        return
    print(f"{'KEY':<5}{'WAVE':<6}{'MODEL':<8}{'STATE':<7}{'TEST':<24}TITLE")
    for t in sorted(tasks.values(), key=lambda t: (t["wave"], t["key"])):
        state = "done" if t["done"] else ("READY" if t["ready"] else "wait")
        print(f"{t['key']:<5}{t['wave']:<6}{t['model']:<8}{state:<7}{t['test']:<24}{t['title']}")
    print(f"\ndone {len(done)}/{len(tasks)}   sanity: " + ", ".join(f"{k}={'ok' if v else 'FAIL'}" for k, v in sanity.items()))
    ready = [t["key"] for t in tasks.values() if t["ready"]]
    print("READY:", " ".join(ready) if ready else "(none)")


if __name__ == "__main__":
    main()
