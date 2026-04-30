# Visible-types pilot — evaluation

Outcome of the comment-shadow pass for the "Make types visible at every use
site" proposal in `llm-language-design.md`. This document captures observations
to help decide whether to lift the convention into the grammar (Option B), keep
it as a comment-only pilot, or revise the proposal.

## Coverage

Mechanical annotations applied via `/tmp/annotate_visible_types.py`:

| Corpus                | Files annotated | Lines annotated |
|-----------------------|-----------------|-----------------|
| tests/samples         | ~330 / 638      | ~730            |
| wisey/ stdlib         | 32 / 43         | 112             |
| samples/ user-facing  | 1 / 2           | 1               |

Plus ~16 manual edits in tests/samples that exercised chains, void calls, and
varied call positions to validate the convention before scripting.

## Patterns annotated

1. `TYPE var = expr-with-call;` — annotated `// -> TYPE` (LHS type).
2. `var = build(TYPE)…onHeap();` and `var = TYPE.create();` — annotated
   `// -> TYPE*` (always builder-output type, even if LHS is a reference).
3. `return expr-with-call;` — annotated `// -> ENCLOSING_RETURN_TYPE`.
4. Multi-line builders/declarations — annotation lands on the closing `;`
   line; builder type takes precedence over LHS type.

## Patterns *not* annotated (intentional)

- `obj.method();` in statement position (return discarded).
- `obj.method()` as an argument to another call (`foo(obj.method())`).
- `obj.method()` in `if` / `while` / boolean / arithmetic position (result
  type implied by context).
- `mField` reads of `this` (field decl is in the same file).
- Trivial literal/identifier RHS (`int x = 5`, `MFoo y = z`).

## Reading observations

### Where it reads well

- Single-line declarations with cross-file callees:
  `MCar* toyota = build(MCar).withMake("Toyota").onHeap();   // -> MCar*`
  The reader sees the result type without resolving `build`/`onHeap`.
- Return statements in non-trivial methods:
  `return mLinkList.getValue();   // -> int`
  The annotation lines up with the function's declared return, so it visibly
  cross-references the signature in one glance.
- Mass-of-builders in a setUp block — alignment to col 70 makes the type
  column visually scannable.

### Where it reads badly

- **Chained calls of 3+ links** (the worst case). See
  `test_linklist.yz` `run()` method: a single `return` statement that was
  3 lines becomes 12 lines, with each `.method()` on its own line and a
  `// -> Type` per link. Mechanically correct, visually noisy. Two options:

  1. Accept the verbosity — chains are exactly where an LLM benefits most
     from per-link type visibility.
  2. Annotate only the final type at the end of the chain.
  3. Forbid chains of 3+ in the language; require named intermediates.

  The doc's "flat over deep" proposal aligns with option 3 — capping
  expression depth might be the cleaner fix than annotating every link.

- **Tautological annotations** when the LHS already says it.
  `int x = foo();   // -> int` is redundant if you can see the LHS.
  Pro: it forces the redundancy to be visible (so a LHS/RHS mismatch can't
  hide). Con: noise. The doc explicitly endorses this redundancy: "Verbosity
  is fine. Repetition is fine. Inference is the enemy."

- **Owner-vs-reference subtype cases**.
  `MShape rectangle = build(MShape).onHeap();   // -> MShape*`
  The annotation says `MShape*` (what the call returns) while the LHS is
  `MShape` (the autocast target). This is intentional — the annotation
  describes the call, not the binding — but a casual reader may be confused
  by the apparent mismatch. May want a clearer convention here.

- **Multi-line statements with the annotation on the `;` line**.
  ```
  long length = input[input.getSize() - 1] == '\0'
    ? (long) ::llvm::function::strlen(input) + 1
    : input.getSize() + 1;                                          // -> long
  ```
  Annotation is on the closing line, far from the LHS. Slightly disorienting
  but works.

## Cases this pilot doesn't cover

- **Argument-position calls.** `foo(bar())` — `bar()`'s return type is implied
  by `foo`'s signature, so we skipped. But for an LLM that doesn't have
  `foo`'s declaration on the screen, this is exactly where the annotation
  helps. A future iteration may want to annotate these too, perhaps with
  the convention `foo(bar() /* -> X */)` (inline) rather than trailing.

- **Single-line chains.** `a.b().c().d()` on one line is common in the
  corpus. We didn't break these into the per-link layout. Doing so
  mechanically would require knowing the intermediate types, which means
  same-file lookup at minimum. The mechanical script declines.

- **Field access through chain.** `obj.field.method()` — script only handles
  the trailing call's binding/return.

## Recommendations

1. **Keep the `// -> Type` convention**. It's low-cost (comments), unenforced,
   and lets us iterate on the convention without compiler churn.
2. **Decide on chains** before lifting to grammar. Either commit to the
   verbose per-link layout, simplify the rule to "annotate the final type
   only," or land the "flat over deep" proposal first and require named
   intermediates.
3. **Decide on owner-vs-reference**. Either change the convention to
   annotate the binding type (`// -> MShape`) or document explicitly that
   the annotation reflects the call's return type (which is what we did).
4. **Don't pursue Option B (grammar) until the above are resolved.** A
   parsed `expr -> Type` annotation that the typechecker enforces will lock
   the convention; revising it later means rewriting source.

## Compile-and-test verification

- `bin/yzc` / `bin/wiseyc` / `bin/wiseylibc` rebuild cleanly with all
  annotated sources.
- `bin/runtests` is 2279 / 2279 passing.
- `samples/HelloWorld` runs end-to-end.
