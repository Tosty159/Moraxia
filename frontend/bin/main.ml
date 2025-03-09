open Lexeparser

let () =
  let args = Sys.argv in
  let argc = Array.length args in
  if argc <> 2 then
    failwith (Printf.sprintf "Usage: %s <moraxia file>" args.(0))
  else begin
    let stream = open_in args.(1) in
    let lex = Lexer.create_lexer stream in
    let rec print_tokens lex =
      let t = Lexer.next_token lex in
      match t with
      | Token.EOF -> print_endline (Token.show_token t)
      | _ -> print_endline (Token.show_token t); print_tokens lex
    in
    print_tokens lex
  end