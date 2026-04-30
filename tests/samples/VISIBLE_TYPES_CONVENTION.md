# Visible-types comment-shadow convention (Option A pilot)

Pilot for the "Make types visible at every use site" proposal in
`llm-language-design.md`. We're testing the annotation as a `// -> Type`
trailing comment first, before deciding whether to lift it into the grammar.

## Rule

Annotate the result type of a call expression when the result is:

- **bound to a variable** — `MFoo* x = foo();  // -> MFoo*`
- **returned** — `return foo();  // -> int`
- **a receiver in a chain of 3+ method calls** — each link gets its own line, each
  with its own `// -> Type` annotation

Skip when the result is:

- used as an argument to another call (the called method's signature already
  pins the expected type)
- unused (statement-position void, or ignored return value)
- pure arithmetic, boolean, comparison, or bitwise on primitives
- a field of `this` (`mField`) — the field's declaration is in the same file
- the trivial RHS of a literal assignment (`int x = 5`)

## Layout

- For single-line calls, append the comment at the end of the line, padded so
  comments line up vertically within a function.
- For chains of 3+, break each `.method()` to its own line, indent under the
  receiver, and put the comment after each call.

## Open questions

- Builders: `build(MFoo).withX(...).onHeap()` is treated as a single chain to
  the final `MFoo*`. Intermediate builder types are unnamed; we only annotate
  the final result. If user-defined generics for builders ever land, revisit.
- Cross-file vs in-file calls: in principle, in-file callees don't need
  annotation (def is on the screen). The convention currently treats them
  uniformly to keep the rule simple. May relax later.
- Argument expressions in nested calls: currently skipped per the rule above.
  An LLM reading `foo(bar())` has to know `bar`'s return type matches `foo`'s
  argument type — this is the cost of the skip. Re-evaluate after pilot.
