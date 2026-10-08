# Build ledger

- E1: dispatched (task-coder-fast)
- E2: dispatched (task-coder-fast)
- E3: dispatched (task-coder-fast)
- E4: dispatched (task-coder-fast)
- E1: complete (8625279)
- E2: complete (62b88bb)
- E3: complete (371afa6)
- E4: complete (4273420) — Ruling (coder): load() returns false on >14 rows/bad row/char; silver reports unbreakable
- E5, E6, E7, E8: dispatched (task-coder-fast)
- Ruling: OOM crash (63 concurrent cc1plus ~18GB from 4 coders x uncapped ninja). Capped coder builds at --parallel 3, task_status at 4, dispatch width 2. E5-E8 had no landed changes; redispatching.
