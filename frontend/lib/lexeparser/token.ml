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
  | Semicolon
  | EOF

let show_token token =
  match token with
  | Int i -> Printf.sprintf "Int(%d)" i
  | Float f -> Printf.sprintf "Float(%f)" f
  | Char ch -> Printf.sprintf "Char(%c)" ch
  | String s -> Printf.sprintf "String(%s)" s
  | Bool b -> Printf.sprintf "Bool(%s)" (string_of_bool b)
  | Operator op -> Printf.sprintf "Operator(%s)" op
  | Keyword kw -> Printf.sprintf "Keyword(%s)" kw
  | Identifier id -> Printf.sprintf "Identifier(%s)" id
  | Punctuation punct -> Printf.sprintf "Punctuation(%s)" punct
  | Semicolon -> "Semicolon"
  | EOF -> "EOF"