type lexer = {
  stream: in_channel;
  mutable is_over: bool;
  mutable unget_stack: char list;
}