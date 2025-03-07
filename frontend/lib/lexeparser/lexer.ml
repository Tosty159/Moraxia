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

let handle_number lex first =
  None

let handle_alphabetic lex first =
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