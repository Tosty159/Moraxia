# Moraxia
A programming language I'm working on :p

Moraxia is math oriented, thus it uses mathematical expressions as code.

## Example code
```
# The 3n+1 conjecture
fn step(x: SetN) -> SetN {
  if (x == 0 mod 2) {
    x / 2
  } else {
    3 * x + 1
  }
}
```
