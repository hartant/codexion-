*This project has been created as part of the 42 curriculum by mbenamar.*

# Codexion

## Description

Codexion is a multithreaded simulation inspired by the classic **Dining Philosophers**
problem: **coders** sit in a circular hub and share a limited pool of **USB dongles** to
compile their code.

Each coder is a POSIX thread cycling through **compiling**, **debugging**, and
**refactoring**. Compiling requires holding two dongles at once (left and right),
shared with neighboring coders. A coder that fails to start compiling again before
`time_to_burnout` **burns out**, ending the simulation.

The project focuses on correctly synchronizing access to shared resources with mutexes
and condition variables, avoiding deadlocks, limiting starvation, and implementing a
custom priority queue (min-heap) for two scheduling policies: **FIFO** and **EDF**.
A dedicated **monitor thread** detects burnout with millisecond precision and triggers a
clean, coordinated shutdown.

## Instructions

### Compilation

```bash
make
```

This builds the `codexion` binary at the root of the repository using `cc` with the flags
`-Wall -Wextra -Werror -pthread`.

Other available Makefile rules:

```bash
make clean    # remove object files
make fclean   # remove object files and the binary
make re       # fclean + full rebuild
```

### Execution

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) in the simulation |
| `time_to_burnout` | Time in ms before an idle coder burns out |
| `time_to_compile` | Time in ms spent compiling |
| `time_to_debug` | Time in ms spent debugging |
| `time_to_refactor` | Time in ms spent refactoring |
| `number_of_compiles_required` | Minimum compiles per coder before the simulation can stop successfully |
| `dongle_cooldown` | Time in ms a dongle stays unavailable after being released |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

### Example runs

```bash
# A single coder can never compile alone -> must burn out
./codexion 1 800 200 200 200 10 0 fifo

# Feasible run, no burnout expected, stops once everyone compiled 10 times
./codexion 5 2000 200 200 200 10 0 fifo

# Same, with the EDF scheduler
./codexion 5 2000 200 200 200 7 0 edf
```

## Resources

### References

- POSIX Threads Programming (LLNL Tutorial) and the relevant man pages
  (`pthread_mutex_init`, `pthread_cond_timedwait`, `clock_gettime`, `gettimeofday`)
- The Dining Philosophers Problem (E. W. Dijkstra, 1965) — basis for the
  resource-ordering strategy used to avoid circular wait
- Earliest Deadline First (EDF) scheduling theory, basis for the `edf` policy
- Binary heap / priority queue data structure background, used to implement the
  custom min-heap (no standard library priority queue, per the subject's constraint)

### How AI was used

An AI assistant (Claude) was used as a **learning and debugging aid**, not as a direct
code generator for graded logic:

- Explaining concepts (threads, stack vs heap, mutexes, condition variables, Coffman's
  deadlock conditions) before any related code was written.
- Guiding the overall architecture (file layout, structs, function responsibilities)
  through discussion; the code itself was written by the student and reviewed after.
- Helping interpret compiler errors, a `gdb` backtrace for a segfault in
  `cleanup_simulation`, and a `ThreadSanitizer` report that revealed an unprotected
  write to `compiles_done`.
- Designing a benchmark test suite (parsing, burnout, feasible/infeasible runs,
  cooldown, FIFO vs EDF, norm checks), executed and interpreted by the student.
- Triaging a Helgrind warning inside `pthread_cond_timedwait`, confirmed as a known
  Helgrind/glibc 2.34+ false positive via `ThreadSanitizer` (0 warnings) and manual
  code review.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions)**: to break *circular wait*, coders
  with an **even** number acquire their **right** dongle first, odd-numbered coders
  acquire their **left** dongle first. This asymmetric order prevents every coder
  from holding one dongle while waiting forever for the other.
- **Starvation mitigation**: dongle access is arbitrated by a custom min-heap per
  dongle, ordered by arrival time (`fifo`) or burnout deadline (`edf`). Under heavy
  artificial contention (very large `dongle_cooldown` relative to `time_to_burnout`),
  occasional starvation was observed and is documented here rather than hidden: a
  coder that finishes its cycle quickly can be re-queued behind requests pending
  since the start of the simulation. This is a known limitation under extreme
  parameters, not a correctness bug in the locking/deadlock logic.
- **Cooldown handling**: each dongle stores `last_release_time`; `take_dongle`
  waits until `dongle_cooldown` ms have elapsed since the last release before
  granting it again.
- **Precise burnout detection**: a monitor thread checks every coder's
  `last_compile_start` against `time_to_burnout`. Every sleep phase is
  interruptible (coders periodically re-check the shared `stop` flag), so a
  detected burnout makes every thread unwind cleanly instead of finishing its
  current phase, guaranteeing `burned out` is always the last line printed.
- **Log serialization**: all output goes through `log_event`, which locks a
  dedicated `print_lock` mutex around `printf`, preventing interleaved lines.

## Thread synchronization mechanisms

- **`pthread_mutex_t` per dongle**: protects each dongle's `available` flag,
  `last_release_time`, and its private waiting-request heap. One mutex per dongle
  (instead of a single global lock) lets coders competing for *different* dongles
  progress in parallel.
- **`pthread_cond_t` per dongle**: used with `pthread_cond_timedwait` so a waiting
  coder sleeps efficiently (no busy-waiting) while still waking automatically once
  its cooldown deadline is reached, even without an explicit broadcast.
- **`stop_lock` mutex**: protects the shared `stop` flag together with
  `last_compile_start` and `compiles_done`. This is the main thread-safe channel
  between coders and the monitor: the monitor sets `stop` under this lock, coder
  threads read it under the same lock and unwind gracefully.
- **`print_lock` mutex**: serializes every `log_event` call, preventing
  interleaved output between coder threads and the monitor.
- **Race condition found and fixed**: an early version incremented
  `coder->compiles_done` *before* acquiring `stop_lock`, while the stop-check read
  it *under* `stop_lock`. `ThreadSanitizer` flagged the unsynchronized read/write;
  the fix moved the increment inside the locked section.
- **Custom min-heap**: implemented from scratch (no standard library priority
  queue) to decide which waiting coder gets a dongle next, ordered by arrival time
  (`fifo`) or burnout deadline (`edf`).