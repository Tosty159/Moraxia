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

let read_optional lex expected =
  let c = read lex in
  if c = expected then true else (unget lex c; false)

let handle_symbol lex first =
  match first with
  (* Operators: *)
  (* Assignment *)
  | '=' -> (* Could be Comparison *)
    let op = "=" in

    if read_optional lex '=' then
      Some (ComparisonOp (op ^ "="))
    else
      Some (AssignmentOp op)
  | ':' -> (* Could be Typing *)
    let op = ":" in

    if read_optional lex '=' then
      Some (AssignmentOp (op ^ "="))
    else
      Some (TypingOp op)
  (* Typing *)
  | '-' -> (* Could be Arithmetic/Assignment *)
    let op = "-" in

    if read_optional lex '>' then
      Some (TypingOp (op ^ ">"))
    else if read_optional lex '=' then
      Some (AssignmentOp (op ^ "="))
    else
      Some (ArithmeticOp op)
  (* Memory *)
  | '&' -> (* Can be BitLogic *)
    if read_optional lex '&' then
      Some (BitLogicOp "&&")
    else
      Some (MemoryOp "&")
  | '@' -> Some (MemoryOp "@")
  (* Arithmetic *)
  | ('+' | '%') as ch -> (* Could be Assignment *)
    let op = String.make 1 ch in

    if read_optional lex '=' then
      Some (AssignmentOp (op ^ "="))
    else
      Some (ArithmeticOp op)
  | ('*' | '/') as ch -> (* Could be duplicated and/or assigment *)
    let op = String.make 1 ch in

    if read_optional lex '=' then
      Some (AssignmentOp (op ^ "="))
    else if read_optional lex ch then
      let dup_op = op ^ op in
      if read_optional lex '=' then
        Some (AssignmentOp (dup_op ^ "="))
      else
        Some (ArithmeticOp dup_op)
    else
      Some (ArithmeticOp op)
  (* BitLogic *)
  | '!' -> (* Could be Comparison *)
    let op = "!" in

    if read_optional lex '=' then
      Some (ComparisonOp (op ^ "="))
    else
      Some (BitLogicOp op)
  | ('|' | '^') as ch -> (* Must be duplicated *)
    let op = String.make 1 ch in

    if read_optional lex ch then
      Some (BitLogicOp (op ^ op))
    else
      let message = Printf.sprintf "Ivalid character: '%c'" ch in
      failwith message
  (* Comparison *)
  | '<' -> (* Could be punctuation *)
      if read_optional lex '[' then
        Some (Punctuation "<[")
      else
        Some (ComparisonOp "<")
  | '~' -> Some (ComparisonOp "~")
  | '>' as ch -> (* Could be followed by '=' *)
    let op = String.make 1 ch in

    let op = if read_optional lex '=' then op ^ "=" else op in

    Some (ComparisonOp op)
  (* Collective *)
  | ('(' | ')' | '[' | ']' | '{' | '}') as ch -> Some (CollectiveOp (String.make 1 ch))
  (* Dot *)
  | '.' ->
    let op = "." in

    let op = if read_optional lex '.' then
      if read_optional lex '.' then
        "..."
      else
        ".."
    else
      op
    in
    
    Some (DotOp op)
  (* Punctuation *)
  | ',' as ch -> Some (Punctuation (String.make 1 ch))
  (* Semicolon *)
  | ';' -> Some (Semicolon)
  | _ -> None

let rec handle_whitespace lex =
  let ch = read lex in
  match ch with
  | ' ' | '\t' | '\n' -> handle_whitespace lex
  | _ -> unget lex ch; ()

let next_token lex =
  if lex.is_over then
    EOF
  else begin
    handle_whitespace lex;
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