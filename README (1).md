# Pascal Lexer

A lexical analyzer written in C++ that tokenizes a Pascal-like programming language. Built as a deep-dive into how real compilers work under the hood — specifically the first stage: breaking raw source code into meaningful tokens.

---

## Background

Compilers and interpreters don't read code the way humans do. The first thing they do is **lex** it — scan through every character and group them into tokens like keywords, variable names, numbers, and operators. This project implements that first stage from scratch.

---

## What it does

Takes any `.pas` source file and outputs the token stream. For example:

**Input:**
```pascal
x := 5 + 10;
```

**Output:**
```
IDENT: <x>
ASSOP: ":="
ICONST: (5)
PLUS: "+"
ICONST: (10)
SEMICOL: ";"
```

It also catches errors early — things like unclosed strings, bad floating-point numbers, and nested comments.

---

## Files

| File | Description |
|------|-------------|
| `lex.h` | Token type definitions and class interface |
| `lex.cpp` | Core lexer logic |
| `main.cpp` | CLI driver with analysis flags |

---

## Build

```bash
g++ -std=c++17 -o lexer lex.cpp main.cpp
```

## Usage

```bash
./lexer <file> [flags]
```

| Flag | Description |
|------|-------------|
| `-all` | Print every token as it's scanned |
| `-ids` | List all unique identifiers and keywords |
| `-num` | List all unique integer and real constants |
| `-str` | List all unique string literals |

```bash
./lexer program.pas -all
./lexer program.pas -ids -num -str
```

---

## Supported tokens

- **Keywords** — `program`, `begin`, `end`, `if`, `then`, `else`, `var`, `const`, `writeln`, `write`, `readln`, `integer`, `real`, `boolean`, `char`, `string`, `and`, `or`, `not`, `div`, `mod`
- **Identifiers** — any user-defined name
- **Integer constants** — `42`, `100`
- **Real constants** — `3.14`, `2.0E-3`, `34.56E+4`
- **String literals** — `'hello world'`
- **Boolean literals** — `true`, `false`
- **Operators** — `+` `-` `*` `/` `:=` `=` `<` `>` `div` `mod` `and` `or` `not`
- **Delimiters** — `, ; ( ) : . { }`
- **Comments** — `{ inline }` and `(* block *)`

---

## Error detection

| Error | Example |
|-------|---------|
| Invalid character | `@`, `#` |
| Newline in string | `'hello` + newline |
| Bad float | `27.57.5` |
| Bad exponent | `3.0Ee`, `0.75E-+2` |
| Unclosed comment | `(* never closed` |
| Nested comment | `(* outer (* inner *)` |

---

## Tech

- C++17
- No external libraries — stdlib only
