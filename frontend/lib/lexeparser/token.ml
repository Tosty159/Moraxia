type token =
  | Int of int
  | Float of float
  | Char of char
  | String of string
  | Bool of bool
  | Operator of string
  | Keyword of string
  | Identifier of string
  | Punctuation of string
  | EOF