# Designing a Language for LLMs

Notes on how to evolve wisey (or design a new language) so that LLMs can read, write, and reason about the code with maximum context and minimum lookup.

## Core principle

**Maximize what an LLM can deduce from a small reading window.**

LLMs read in a sliding context. Anything that forces them to "look up the definition elsewhere" is expensive — they either guess, or you pay tokens to fetch the missing context. So the design goal is: every line of code should be near-self-describing without leaving the screen.

Verbosity is fine. Repetition is fine. Inference is the enemy.

This principle pushes against several things modern languages are proud of:

- Type inference
- Method chaining
- Implicit conversions
- Operator overloading
- Deep inheritance
- Macros and decorators
- Ambient context (implicit `this`, implicit imports)
- Dynamic dispatch

Each of these moves information off-screen. None of them help an LLM, and many of them hurt humans too.

## What to keep from wisey

The prefix conventions (`M`/`C`/`N`/`I`, `mField`, `ALL_CAPS`) are one of the most LLM-friendly things in the language. They are disambiguating tokens at the point of use. The earlier criticism of them was wrong in an LLM-first context — keep them, and push them further.

Also keep:

- Single-owner ownership (`*`) — explicit ownership at every use site.
- Immutability of models — fewer mutation effects to track.
- `throws` clauses — explicit effects in signatures.
- One-class-per-file — predictable file lookup.
- The model/controller/node trichotomy — a clear separation of concerns the LLM can rely on.

## Concrete proposals

### Make types visible at every use site

Forbid type inference for non-literal expressions. `let x = foo()` is hostile; `int x = foo()` is friendly. Better: annotate the return type at the call site as well, e.g. `int x = foo() -> int`, so the LLM can verify locally that types line up without resolving `foo`.

The cost is keystrokes the LLM no longer has to spend, because the LLM is writing them.

### Make effects part of the signature, checked at the call site

Wisey already has `throws`. Extend it. Every method declares what it does:

- `reads` — which state it observes
- `writes` — which state it mutates
- `allocates` — whether it heap-allocates
- `blocks_on` — synchronization primitives it can wait on
- `io` — file, network, terminal effects
- `thread_crosses` — whether it sends across thread boundaries

At each call site, the compiler checks that the caller's declared effect set is a superset of the callee's. The LLM can read any function and know exactly what it does without tracing.

### Kill cross-file ambiguity

Drop import aliasing. Either:

- Require fully-qualified names at every use site (`wisey.lang.MString` everywhere, not `MString`), or
- Accept a leading `import` and have the compiler/LSP insert the FQN as a virtual annotation that the LLM sees in a render mode.

Same idea for method calls: methods on objects always show the receiving type — `car:MCar.getYear()` rather than `car.getYear()`. Wisey's prefix convention is a weak version of this; go all the way.

### Symbol set: one symbol, one meaning

The "make types visible" and "kill cross-file ambiguity" proposals stack several syntactic markers — receiver type, return type, fully-qualified name, method call. If they share symbols, the LLM (and reader) has to disambiguate by position. The cleaner discipline is one symbol per role.

| Symbol | Meaning |
|---|---|
| `:` | type assertion — receiver type before a method call (`recv:Type->method()`) |
| `~>` | return-type assertion (`foo() ~> int`) |
| `->` | method call — instance, static, or annotated (`obj->method(args)`) |
| `.` | structural lookup — package paths, inner types, constants, builder/injector chains (`wisey.threads.CCallStack`, `MType.CONST`, `build(M).withX().onHeap()`) |

The previous Wisey form overloaded `.` for FQN, inner-type, method-call, constant, and builder argument; the visible-types pilot added `:` for receiver and `->` for return. Splitting these makes every token unambiguous on its own:

```
mVector:wisey.data.COwnerVector*->getAt(i) ~> ::wisey::object
recv:CPoolThread.CPoolThreadRunner*->createThread(this)
((MCar) mVector:CV*->getAt(i)):MCar->getYear() ~> int
```

Reading rule: `.` never executes anything; `->` always does. `:` is the type. `~>` is what comes back. With this split, an LLM tokenizing a single line can determine the role of every separator from the symbol alone, never from position-counting or context-disambiguation.

This was painful to converge on. The first iteration of the visible-types pilot used `recv:Type:method()` (two colons framing the type) to free `.` for inner-type chains, but the same character in two roles forces position-counting. The second iteration considered `recv:Type$method()` (asymmetric: `$` only as inner-type separator), but then the same inner type was written `Outer.Inner` in declarations and `Outer$Inner` in receiver annotations — same type, two spellings. The unified design above avoids both: every symbol has one meaning, used the same way in every position.

This supersedes the spelling shown in the earlier examples in this doc — `int x = foo() -> int` becomes `int x = foo() ~> int`, and `car:MCar.getYear()` becomes `car:MCar->getYear()`.

### One canonical way to do everything

- No overloading. `add_int_int`, `add_float_float`, `concat_str_str` are separate names.
- No implicit conversions, ever. `1 + 2.0` is a type error; you write `int_to_float(1) + 2.0`.

This sounds painful and is the right tradeoff: the LLM never has to disambiguate intent, and search/grep finds every caller of a specific operation.

### Generics only on a fixed stdlib set, never inferred

The "no overloading" rule above is the right default for the function level. Extending it naively to the type level would explode the surface area — every container needs a per-type version (`IntList`, `MCarList`, `MStringList`, `MCarOwnerList`, ...). The combinatorics are bad enough that the language needs a narrow exception.

The exception is Go-shaped, not ML/Rust-shaped:

- A fixed, hardcoded stdlib set gets type parameters: `List<T>`, `Map<K,V>`, `Set<T>`, `Result<T,E>`, `Option<T>`, `Iterator<T>`, `Channel<T>`. Nothing else.
- No user-defined generic types or functions. If your domain wants a tree, you write `MUserTree`; if you want a different tree, you write `MOrderTree`. The boilerplate is the LLM's problem, not the reader's.
- No type inference. `List<int>` is always written at the use site; `List` alone is a syntax error. The reader sees the element type in the same token as the container.
- No typeclasses, traits, constraints, higher-kinded types, or associated types. The stdlib generics are parametric — the same code works for any `T` because they don't *do* anything that depends on `T`'s shape (storing, iterating, moving).
- The compiler monomorphizes. Each instantiation produces its own object code, its own effect signature, its own grep target. `grep "List<int>::push"` finds every int-list push call.

What this banishes is the standard ML/Rust shape: user-defined generic items, type inference at call sites, and trait/typeclass-driven dispatch. That's the version optimized for human readers with IDEs, and it's where the LLM-reading story falls apart fastest.

The harder question — whether to permit user-defined generic *containers* (a custom `MCircularBuffer<T>`) — is left open. The conservative answer is no: write `MCircularBufferInt`, `MCircularBufferMCar`, and let the LLM generate them. The pragmatic answer is yes-but-only-if the type uses its parameter purely structurally (storage, iteration, move) and never calls methods on it. Pick when one of these actually bites.

### Errors as values, not exceptions

Replace `throws` with `Result<T, ErrEnum>` returns. Control flow stays local; the LLM doesn't have to mentally simulate stack unwinding through five frames to know what can fail.

The exception machinery already in wisey becomes the implementation of `Result`'s `?` operator.

### Bake examples and contracts into the grammar, machine-checked

Every public method requires a doc block with:

- `@purpose` — one line summary
- `@example` — executable, run as a test
- `@requires` / `@ensures` — predicates the typechecker enforces

The LLM reads the contract block and knows the function. The compiler keeps it honest. Documentation that drifts is the worst kind of context for an LLM — making it executable fixes that.

### Eliminate ambient context

Wisey has implicit `thread` and `callstack`. Make them explicit parameters. Yes, every function gains an `IThread thread` argument. Yes, that is verbose. The LLM can now see thread-crossing without inference.

### Flat over deep

- Cap interface inheritance at one level, or disallow it (composition only).
- Cap nesting depth.
- Cap method length.

Mechanical limits the compiler enforces — because LLMs degrade nonlinearly with nesting depth and method length.

### Local re-declaration in long blocks

If a variable is used more than N lines after declaration, the compiler requires you to re-annotate its type: `// type: int x` as a checked comment. Ugly to write, a gift to read.

### Push the prefix convention further

Owner refs get a suffix: `MCar* tesla_o`. Regular refs: `tesla_r`. Immutable arrays: `args_ia`. Now every variable use site carries its kind.

Hungarian gets a bad rap because in a non-LLM world the IDE shows you the type on hover. In an LLM world the LLM is the IDE, and it doesn't get a hover — it gets the token.

## Memory management

Static safety is LLM safety, so the memory model has to be built on what the compiler can prove rather than what the runtime can catch. That rules out tracing GC (the static signal vanishes at runtime), implicit refcount on the default path (every call hides an atomic operation), and Rust-style lifetime elision (off-screen reasoning, exactly what the core principle bans).

The chosen discipline is **single-owner moves with named regions for everything else**, plus opt-in shared/weak references for genuinely unpredictable lifetimes.

### Three things, visible at every use site

Every memory-relevant value is one of three kinds, marked in both its type and its suffix:

- **Owner** — `MCar* tesla_o`. Sole owner; destroyed when its region drops or it's moved out.
- **Borrow** — `MCar& tesla_b`. Call-duration reference; cannot outlive the owner.
- **Region** — `region req_r`. A named bag of memory declared by some scope and dropped at its end.

The `_o`/`_b`/`_r` suffixes are the same Hungarian-for-LLMs idea as the existing `M`/`C`/`N`/`I` prefixes — the kind of the variable is in the token, not in tooling.

### Allocation always names a region

There is no default heap. Every `new` declares its region: `new MUser("vlad") in req_r`. A function's signature declares which regions it can allocate into as part of its effect set: `allocates(in: req_r)`. A function declared `allocates(none)` cannot call `new`. A function declared `allocates(in: req_r)` cannot escape an allocation into a region whose lifetime is shorter or unrelated. The region annotation is the borrow-checker piece, expressed as a named value rather than an anonymous `'a` lifetime variable.

### Regions form an outlives tree

Regions are strictly tree-shaped. A reference may only point "up" the tree — into a region that outlives the holder's region — never sideways or down. Storing a `req_r` borrow inside a `perm_r` object is a compile error; the reverse is fine, because `perm_r` outlives `req_r` for all of `req_r`'s scope. When a region drops, every owner allocated in it is destroyed in declaration order. The compiler has already proven that nothing outside the region holds a pointer in, so a dangling reference into a dropped region is structurally impossible: a compile error, never a runtime fault.

### Cross-region moves are explicit

The most common god-region failure mode is silent cross-region transfer — code that allocates in `req_r`, then "puts it in the cache," and ownership has invisibly migrated into `cache_r`. The remedy is to make every cross-region transfer a visible token: `move user_o into cache_r`. Every cache insertion, every promotion to longer-lived storage, becomes grep-able. An LLM auditing for accidental retention has one string to search for.

### Refcount and weak references are opt-in and visibly named

Single ownership covers most allocations and regions cover most lifetimes, so the default code path never bumps an atomic counter. For the cases where lifetime is genuinely observational rather than structural — caches, sessions, debounced subscriptions, parts of a UI tree — the language provides `MShared<T>` (refcounted shared ownership) and `MWeak<T>` (non-owning, nullable, observes drop). Both come from the fixed stdlib generics set; their names appear at every use site, so the cost is visible. Cycles are forbidden by construction unless at least one edge is an `MWeak`.

### Region governance: avoiding the god region

The classic failure mode is one big `permanent_r` that accumulates everything and only frees at program shutdown. The language pushes back at three levels. First, `permanent_r` is not ambient — it's declared once at program start, owned by `main`, and only functions that explicitly take it as a parameter can allocate in it; reviewers can grep for those functions. Second, region declarations carry a discipline tag: `region req_r bound 1MB` traps if it grows past its bound, `region cache_r monotonic` is allowed to grow but the type marks it specially, `region perm_r frozen_after_init` rejects allocations past startup. Third, long-lived state is subdivided into one region per subsystem (config, plugin, cache, connection pool) rather than one big permanent region, so subsystems can be torn down and rebuilt independently.

### Compile-time replaces three runtime exceptions

With this discipline, three of wisey's existing runtime exceptions disappear:

- `MNullPointerException` — every owner is non-null at construction; `Option<T>` is the way to express absence.
- `MReferenceCountException` — single ownership plus region drop replace the refcount-tracking machinery on the default path; on the opt-in `MShared` path, drop happens only when the refcount reaches zero, so the analogous error doesn't arise.
- `MArrayIndexOutOfBoundsException` — for fixed-size arrays the index is checked at compile time; for dynamic indexing the result type is `Option<T>` and the absence case is handled in source.

A class of errors that used to surface in production logs now shows up in tooling the LLM can see during writing. **Static safety is LLM safety.**

### Lineage and prior art

The design is in the family of Cyclone (the safe-C dialect that pioneered explicit regions), Vale (the current language closest to programmer-named regions plus a borrow checker), MLKit (Tofte-Talpin region inference for SML), and the library-level arena tradition (Apache APR pools, Zig allocators, Erlang per-process heaps).

The novel piece, relative to those, is the **no-inference rule**. Cyclone and Rust both leaned heavily on inference because they were optimizing keystrokes for human writers. The LLM-first inversion is to make every region, every owner, and every borrow visible at every use site, and to make the writer pay the keystroke tax that the reader gets back tenfold.

## The meta-move

Stop optimizing the source language for human keystroke economy.

That tradeoff was right when humans typed every character. It is wrong when an LLM types most of them and humans (and other LLMs) read them ten times more often than they are written.

Optimize for reading density of meaning, not writing density of characters.

## Open questions

- Reading-friendly vs writing-friendly: they overlap a lot but not perfectly. For writing the LLM also wants tolerant grammars, fast static feedback, and copy-pasteable patterns. Worth pulling these apart in a follow-up.
- How much of this is best done in the language vs in a "view layer" the LLM sees (where the compiler renders FQNs, inferred types, and effect sets as virtual annotations)? Some of these proposals could be source-invariant and tooling-only.
- A self-hosting frontend in the new language would be the strongest test of whether the readability tax is bearable at scale.
