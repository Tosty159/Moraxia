open Token

type lexer = {
  stream: in_channel;
  mutable is_over: bool;
  mutable unget_stack: char list;
}

let create_lexer stream =
  {
    stream = stream;
    is_over = false;
    unget_stack = [];
  }

let read lex =
  match lex.unget_stack with
  | [] -> input_char lex.stream
  | hd :: tl -> lex.unget_stack <- tl; hd

let unget lex ch =
  lex.unget_stack <- ch :: lex.unget_stack

let is_digit ch =
  '0' <= ch && ch <= '9'

let is_alpha ch =
  'a' <= ch && ch <= 'z' || 'A' <= ch && ch <= 'Z' || ch = '_'

let is_alnum ch =
  is_alpha ch || is_digit ch

let handle_number lex first =
  if is_digit first then
    let rec read_number lex number =
      match read lex with
      | ch when is_digit ch -> read_number lex (number ^ (String.make 1 ch))
      | ch -> unget lex ch; number
    in
    let num = read_number lex (String.make 1 first) in
    Some (Int (int_of_string num))
  else
    None

let handle_alphabetic lex first =
  if is_alpha first then
    let rec read_alpha lex name =
      match read lex with
      | ch when is_alnum ch -> read_alpha lex (name ^ (String.make 1 ch))
      | ch -> unget lex ch; name
    in
    let name = read_alpha lex (String.make 1 first) in
    Some (Identifier name)
  else
    None

let handle_symbol lex first =
  None

let next_token lex =
  if lex.is_over then
    EOF
  else begin
    let ch = read lex in
    match handle_number lex ch with
    | Some num -> num
    | None ->
      match handle_alphabetic lex ch with
      | Some alpha -> alpha
      | None ->
        match handle_symbol lex ch with
        | Some sym -> sym
        | None ->
          let message = Printf.sprintf "Invalid character: %c" ch in
          failwith message
  end