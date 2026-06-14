#include "includes.hpp"

bool repl = 0;

Token next(std::vector<Token>& tokens) {
  if (tokens.empty()) return Token();
  Token token = tokens.back();
  tokens.pop_back();
  return token;
}

Token peek(std::vector<Token>& tokens) {
  return tokens.empty() ? Token() : tokens.back();
}

short prefix(Symbol op) {
  switch (op) {
    case Symbol::Add:
    case Symbol::Sub:
    case Symbol::Bnot:
    case Symbol::Lnot:
         return 0x0017;
    default:
      return 0;
  }
}

short infix(Symbol op) {
  switch (op) {
    case Symbol::Pow:
      return 0x1615;
    case Symbol::Mul:
    case Symbol::Div:
    case Symbol::Mod:
      return 0x1314;
    case Symbol::Add:
    case Symbol::Sub:
      return 0x1112;
    case Symbol::Shl:
    case Symbol::Shr:
      return 0x0f10;
    case Symbol::Lt:
    case Symbol::Le:
    case Symbol::Gt:
    case Symbol::Ge:
      return 0x0d0e;
    case Symbol::Ne:
    case Symbol::Eq:
      return 0x0b0c;
    case Symbol::Band: return 0x090a;
    case Symbol::Bxor: return 0x0708;
    case Symbol::Bor: return 0x0506;
    case Symbol::Land: return 0x0304;
    case Symbol::Lor: return 0x0102;
    default:
      return 0;
  }
}

std::vector<std::string> names = {"exit", "out", "outln", "input", "bool", "int", "str", "clock", "sleep", "pi", "e", "sin", "cos", "tan", "asin", "acos", "atan"};

void expr(std::vector<Token>& tokens, std::vector<Op>& output, short expr_bp, size_t& stack_size, size_t& stack_max_size);

void expr_stmt(std::vector<Token>& tokens, std::vector<Op>& output, size_t& stack_size, size_t& stack_max_size, bool in_main = false) {
  if (!in_main) {
    if (peek(tokens).type == TokenType::Null)
      if (repl) throw 1;
      else throw "SyntaxError: statement not terminated, expect '{'";
    if (next(tokens).val.sym != Symbol::Lbkl) {
      throw "SyntaxError: bad operator, expect '{'";
    }
  }
  while (1) {
    if (peek(tokens).type == TokenType::Null)
      if (in_main) return;
      else if (repl) throw 1;
      else throw "SyntaxError: '{' not closed, expect '}'";
    if (peek(tokens).type != TokenType::Symbol) goto L2;
    if (peek(tokens).val.sym == Symbol::Rbkl)
      if (in_main) throw "SyntaxError: bad operator";
      else {next(tokens); break;}
    if (peek(tokens).val.sym == Symbol::Lbkl)
      expr_stmt(tokens, output, stack_size, stack_max_size, false);
    
    else if (peek(tokens).val.sym == Symbol::If) {
      std::vector<size_t> jmps_to_end;
      L1:
      next(tokens);
      expr(tokens, output, 0, stack_size, stack_max_size);
      size_t jmp_index = output.size();
      output.emplace_back(OpType::Jiff);
      stack_size--;
      expr_stmt(tokens, output, stack_size, stack_max_size, false);
      if (peek(tokens).type == TokenType::Symbol)
      if (peek(tokens).val.sym == Symbol::Elif) {
        jmps_to_end.push_back(output.size());
        output.emplace_back(OpType::Jmp);
        output[jmp_index].val.size = output.size() - jmp_index;
        goto L1;
      } else if (peek(tokens).val.sym == Symbol::Else) {
        next(tokens);
        jmps_to_end.push_back(output.size());
        output.emplace_back(OpType::Jmp);
        output[jmp_index].val.size = output.size() - jmp_index;
        expr_stmt(tokens, output, stack_size, stack_max_size, false);
      } else output[jmp_index].val.size = output.size() - jmp_index;
      else output[jmp_index].val.size = output.size() - jmp_index;
      for (size_t i : jmps_to_end)
        output[i].val.size = output.size() - i;
    }
    else if (peek(tokens).val.sym == Symbol::While) {
      next(tokens);
      std::vector<Op> cond;
      size_t cond_index = output.size();
      expr(tokens, cond, 0, stack_size, stack_max_size);
      size_t body_index = output.size();
      output.emplace_back(OpType::Jmp);
      expr_stmt(tokens, output, stack_size, stack_max_size, false);
      output[body_index].val.size = output.size() - body_index;
      output.insert(output.end(), cond.begin(), cond.end());
      output.emplace_back(OpType::Jift, cond_index - output.size() + 1);
      stack_size--;
    } else goto L2;

    if (peek(tokens).type == TokenType::Null)
      if (in_main) return;
      else if (repl) throw 1;
      else throw "SyntaxError: '{' not closed, expect '}'";
    while (peek(tokens).type == TokenType::Symbol && peek(tokens).val.sym == Symbol::Eos) next(tokens);
    continue;

    L2:
    expr(tokens, output, 0, stack_size, stack_max_size);
    if (peek(tokens).type == TokenType::Null)
      if (in_main) return;
      else if (repl) throw 1;
      else throw "SyntaxError: '{' not closed, expect '}'";
    if (peek(tokens).val.sym != Symbol::Rbkl && peek(tokens).val.sym != Symbol::Eos) throw "SyntaxError: bad operator";
    while (peek(tokens).type == TokenType::Symbol && peek(tokens).val.sym == Symbol::Eos) next(tokens);
    if (output.back().type == OpType::Asg) output.back().type = OpType::AsgPop;
    else output.emplace_back(OpType::Pop);
    stack_size--;
  }
}

void expr(std::vector<Token>& tokens, std::vector<Op>& output, short min_bp, size_t& stack_size, size_t& stack_max_size) {
  Token token = next(tokens);
  switch (token.type) {
    case TokenType::Null:
      if (repl) throw 1;
      else throw "SyntaxError: statement not terminated, expect any value";

    case TokenType::Atom:
      switch (token.val.atom.type) {
        case AtomType::Null:
          output.emplace_back(OpType::Null);
          break;
        case AtomType::I64:
          output.emplace_back(token.val.atom.val.i64);
          break;
        case AtomType::F64:
          output.emplace_back(token.val.atom.val.f64);
          break;
        case AtomType::Str:
          output.emplace_back(token.val.atom.val.str);
          break;
      }
      if (stack_max_size < ++stack_size) stack_max_size = stack_size;
      break;

    case TokenType::Symbol:
      switch (token.val.sym) {
        case Symbol::Lpar:
          expr(tokens, output, 0, stack_size, stack_max_size);
          if (peek(tokens).type == TokenType::Null)
            if (repl) throw 1;
            else throw "SyntaxError: '(' not closed, expect ')'";
          if (next(tokens).val.sym != Symbol::Rpar) throw "SyntaxError: bad operator";
          break;

        case Symbol::Lbrk: {
          size_t count = 0;
          while (1) {
            if (peek(tokens).val.sym == Symbol::Rbrk) {
              next(tokens);
              break;
            }
            expr(tokens, output, 0, stack_size, stack_max_size);
            count++;
            Token token = next(tokens);
            if (token.type == TokenType::Null)
              if (repl) throw 1;
              else throw "SyntaxError: '[' not closed, expect ']'";
            if (token.val.sym == Symbol::Rbrk) break;
            if (token.val.sym != Symbol::Sep) throw "SyntaxError: bad operator";
          }
          output.emplace_back(OpType::List, count);
          if (stack_max_size < (stack_size += 1-count)) stack_max_size = stack_size;
          break;
        }

        default:
          short bp = prefix(token.val.sym);
          if (!bp) throw "SyntaxError: bad operator, expect prefix operator";
          if (token.val.sym == Symbol::Add || token.val.sym == Symbol::Sub) {
            output.emplace_back(0ll);
            if (stack_max_size < ++stack_size) stack_max_size = stack_size;
          }
          expr(tokens, output, bp&255, stack_size, stack_max_size);
          output.emplace_back((OpType)(char)token.val.sym);
          stack_size--;
      }
      break;

    case TokenType::Id:
      std::vector<std::string>::iterator it = std::find(names.begin(), names.end(), token.val.id);
      size_t index;
      if (it != names.end())
        index = it - names.begin();
      else {
        names.emplace_back(token.val.id);
        index = names.size() - 1;
      }
      if (peek(tokens).type == TokenType::Symbol && peek(tokens).val.sym == Symbol::Asg) {
        next(tokens);
        expr(tokens, output, 0, stack_size, stack_max_size);
        output.emplace_back(OpType::Asg, index);
      } else {
        output.emplace_back(OpType::Var, index);
        if (stack_max_size < ++stack_size) stack_max_size = stack_size;
      }
      break;

  }

  while (1) {
    Token token = peek(tokens);
    if (token.type == TokenType::Null) break;
    if (token.type != TokenType::Symbol) throw "SyntaxError: bad operator, expect a value";
    switch (token.val.sym) {
      case Symbol::Lpar: {
        next(tokens);
        size_t count = 0;
        while (1) {
          if (peek(tokens).val.sym == Symbol::Rpar) {
            next(tokens);
            break;
          }
          expr(tokens, output, 0, stack_size, stack_max_size);
          count++;
          Token token = next(tokens);
          if (token.type == TokenType::Null)
            if (repl) throw 1;
            else throw "SyntaxError: '(' not closed, expect ')'";
          if (token.val.sym == Symbol::Rpar) break;
          if (token.val.sym != Symbol::Sep) throw "SyntaxError: bad operator";
        }
        output.emplace_back(OpType::Call, count);
        stack_size -= count;
        break;
      }
      case Symbol::Lbrk:
        next(tokens);
        expr(tokens, output, 0, stack_size, stack_max_size);
        if (peek(tokens).type == TokenType::Null)
          if (repl) throw 1;
          else throw "SyntaxError: '[' not closed, expect ']'";
        if (next(tokens).val.sym != Symbol::Rbrk) throw "SyntaxError: bad operator";
        // TODO: AsgInd
        output.emplace_back(OpType::Ind);
        stack_size--;
        break;
      case Symbol::Qst: {
        if (0 < min_bp) return;
        next(tokens);
        size_t index = output.size();
        output.emplace_back(OpType::Jiff);
        stack_size--;
        expr(tokens, output, 0, stack_size, stack_max_size);
        if (peek(tokens).type == TokenType::Null)
          if (repl) throw 1;
          else throw "SyntaxError: '?' not closed, expect ':'";
        size_t index2 = output.size();
        output.emplace_back(OpType::Jmp);
        stack_size--;
        output[index].val.size = output.size() - index;
        if (next(tokens).val.sym != Symbol::Cmn) throw "SyntaxError: bad operator";
        expr(tokens, output, 0, stack_size, stack_max_size);
        output[index2].val.size = output.size() - index2;
        break;
      }
      default:
      short bp = infix(token.val.sym);
      if (!bp) return;
      if (bp>>8 < min_bp) return;
      next(tokens);
      if (token.val.sym == Symbol::Land || token.val.sym == Symbol::Lor) {
        size_t index = output.size();
        output.emplace_back(token.val.sym == Symbol::Land ? OpType::Land : OpType::Lor);
        stack_size--;
        expr(tokens, output, bp&255, stack_size, stack_max_size);
        output[index].val.size = output.size() - index;
        continue;
      }
      expr(tokens, output, bp&255, stack_size, stack_max_size);
      output.emplace_back((OpType)(char)token.val.sym);
      stack_size--;
    }
  }
}

std::vector<Atom> vars = {f_exit, f_out, f_outln, f_input, f_bool, f_int, f_str, f_clock, f_sleep, 3.1415926, 2.7182818, f_sin, f_cos, f_tan, f_asin, f_acos, f_atan};

void eval(const std::vector<Op>& cmds, Atom *stack) {
  Atom *pvar = &vars[0];
  Atom *stack_top = stack;
  std::string err;

  for (const Op *p_op = &cmds[0];; p_op++) {
    L1:
    Op cmd = *p_op;
    switch (cmd.type) {
      case OpType::Lnot: {
        Atom *a = stack_top - 1;
        switch (a->type) {
          case AtomType::Null:
            a->type = AtomType::I64;
            a->val.i64 = 1;
            break;
          case AtomType::I64:
            a->val.i64 = !a->val.i64;
            break;
          case AtomType::F64:
            a->type = AtomType::I64;
            a->val.i64 = !a->val.f64;
            break;
          case AtomType::Str: {
            bool b = a->val.str.empty();
            a->val.str.std::string::~string();
            a->type = AtomType::I64;
            a->val.i64 = b;
            break;
          }
          case AtomType::List: {
            bool b = a->val.list.empty();
            a->val.list.std::vector<Atom>::~vector();
            a->type = AtomType::I64;
            a->val.i64 = b;
            break;
          }
          case AtomType::StdFn:
            a->type = AtomType::I64;
            a->val.i64 = 1;
            break;
          default: {err = "TypeError: bad value type for operator '!'"; goto E2;}
        }
        break;
      }
      case OpType::Bnot: {
        Atom* a = stack_top - 1;
        if (a->type == AtomType::I64) a->val.i64 = ~a->val.i64;
        else {err = "TypeError: bad value type for operator '~'"; goto E2;}
        break;
      }
      
      case OpType::Pow: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = pow(a->val.i64, b->val.i64);
          else if (b->type == AtomType::F64)
            a->type = AtomType::F64,
            a->val.f64 = pow(a->val.i64, b->val.f64);
          else {err = "TypeError: bad value type for operator '**'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.f64 = pow(a->val.f64, b->val.i64);
          else if (b->type == AtomType::F64)
            a->val.f64 = pow(a->val.f64, b->val.f64);
          else {err = "TypeError: bad value type for operator '**'"; goto E1;}
        else {err = "TypeError: bad value type for operator '**'"; goto E1;}
        break;
      }
      case OpType::Mul: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 *= b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::F64,
            a->val.f64 = a->val.i64 * b->val.f64;
          else {err = "TypeError: bad value type for operator '*'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.f64 *= b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.f64 *= b->val.f64;
          else {err = "TypeError: bad value type for operator '*'"; goto E1;}
        else if (a->type == AtomType::Str && b->type == AtomType::I64) {
          if (b->val.i64 < 0) {err = "ArgError: bad argument range for operator '*'"; goto E2;}
          if (b->val.i64 == 0) {a->val.str = ""; break;}
          std::string s = a->val.str;
          while (--b->val.i64) a->val.str += s;
        }
        else {err = "TypeError: bad value type for operator '*'"; goto E1;}
        break;
      }
      case OpType::Div: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 /= b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::F64,
            a->val.f64 = a->val.i64 / b->val.f64;
          else {err = "TypeError: bad value type for operator '/'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.f64 /= b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.f64 /= b->val.f64;
          else {err = "TypeError: bad value type for operator '/'"; goto E1;}
        break;
      }
      case OpType::Mod: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 % b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::F64,
            a->val.f64 = fmod(a->val.i64, b->val.f64);
          else {err = "TypeError: bad value type for operator '%'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.f64 = fmod(a->val.f64, b->val.i64);
          else if (b->type == AtomType::F64)
            a->val.f64 = fmod(a->val.f64, b->val.f64);
          else {err = "TypeError: bad value type for operator '%'"; goto E1;}
        break;
      }

      case OpType::Add: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 += b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::F64,
            a->val.f64 = a->val.i64 + b->val.f64;
          else {err = "TypeError: bad value type for operator '+'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.f64 += b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.f64 += b->val.f64;
          else {err = "TypeError: bad value type for operator '+'"; goto E1;}
        else if (a->type == AtomType::Str && b->type == AtomType::Str) {a->val.str += b->val.str;
          b->val.str.std::string::~string();
        }
        else if (a->type == AtomType::List && b->type == AtomType::List) {
          a->val.list.insert(a->val.list.end(), std::make_move_iterator(b->val.list.begin()), std::make_move_iterator(b->val.list.end()));
          b->val.list.std::vector<Atom>::~vector();
        }
        else throw "TypeError: bad value type for operator '+'";
        break;
      }
      case OpType::Sub: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 -= b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::F64,
            a->val.f64 = a->val.i64 - b->val.f64;
          else {err = "TypeError: bad value type for operator '-'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.f64 -= b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.f64 -= b->val.f64;
          else {err = "TypeError: bad value type for operator '-'"; goto E1;}
        else throw "TypeError: bad value type for operator '-'";
        break;
      }

      case OpType::Shl: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64 && b->type == AtomType::I64)
          a->val.i64 <<= b->val.i64;
        else {err = "TypeError: bad value type for operator '<<'"; goto E1;}
        break;
      }
      case OpType::Shr: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64 && b->type == AtomType::I64)
          a->val.i64 >>= b->val.i64;
        else {err = "TypeError: bad value type for operator '>>'"; goto E1;}
        break;
      }

      case OpType::Lt: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 < b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.i64 = a->val.i64 < b->val.f64;
          else {err = "TypeError: bad value type for operator '<'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.f64 < b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 < b->val.f64;
          else {err = "TypeError: bad value type for operator '<'"; goto E1;}
        else throw "TypeError: bad value type for operator '<'";
        break;
      }
      case OpType::Le: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 <= b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.i64 = a->val.i64 <= b->val.f64;
          else {err = "TypeError: bad value type for operator '<='"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 <= b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 <= b->val.f64;
          else {err = "TypeError: bad value type for operator '<='"; goto E1;}
        else {err = "TypeError: bad value type for operator '<='"; goto E1;}
        break;
      }
      case OpType::Gt: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 > b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.i64 = a->val.i64 > b->val.f64;
          else {err = "TypeError: bad value type for operator '>'"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.f64 > b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 > b->val.f64;
          else {err = "TypeError: bad value type for operator '>'"; goto E1;}
        else {err = "TypeError: bad value type for operator '>'"; goto E1;}
        break;
      }
      case OpType::Ge: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 >= b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.i64 = a->val.i64 >= b->val.f64;
          else {err = "TypeError: bad value type for operator '>='"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 >= b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 >= b->val.f64;
          else {err = "TypeError: bad value type for operator '>='"; goto E1;}
        else {err = "TypeError: bad value type for operator '>='"; goto E1;}
        break;
      }
      case OpType::Ne: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 != b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.i64 = a->val.i64 != b->val.f64;
          else {err = "TypeError: bad value type for operator '!='"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 != b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 != b->val.f64;
          else {err = "TypeError: bad value type for operator '!='"; goto E1;}
        else {err = "TypeError: bad value type for operator '!='"; goto E1;}
        break;
      }
      case OpType::Eq: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64)
          if (b->type == AtomType::I64)
            a->val.i64 = a->val.i64 == b->val.i64;
          else if (b->type == AtomType::F64)
            a->val.i64 = a->val.i64 == b->val.f64;
          else {err = "TypeError: bad value type for operator '=='"; goto E1;}
        else if (a->type == AtomType::F64)
          if (b->type == AtomType::I64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 == b->val.i64;
          else if (b->type == AtomType::F64)
            a->type = AtomType::I64,
            a->val.i64 = a->val.f64 == b->val.f64;
          else {err = "TypeError: bad value type for operator '=='"; goto E1;}
        else {err = "TypeError: bad value type for operator '=='"; goto E1;}
        break;
      }

      case OpType::Band: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64 && b->type == AtomType::I64)
          a->val.i64 &= b->val.i64;
        else if (a->type == AtomType::List) {
          a->val.list.push_back(std::move(*b));
          b->~Atom();
        }
        else {err = "TypeError: bad value type for operator '&'"; goto E1;}
        break;
      }
      case OpType::Bxor: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64 && b->type == AtomType::I64)
          a->val.i64 ^= b->val.i64;
        else {err = "TypeError: bad value type for operator '^'"; goto E1;}
        break;
      }
      case OpType::Bor: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::I64 && b->type == AtomType::I64)
          a->val.i64 |= b->val.i64;
        else {err = "TypeError: bad value type for operator '|'"; goto E1;}
        break;
      }

      case OpType::Ind: {
        Atom* b = --stack_top;
        Atom* a = stack_top-1;
        if (a->type == AtomType::Str && b->type == AtomType::I64) {
          if (b->val.i64 >= a->val.str.size()) {err = "IndexError: index out of range"; goto E1;}
          a->val.str = a->val.str[b->val.i64];
        } else if (a->type == AtomType::List && b->type == AtomType::I64) {
          if (b->val.i64 >= a->val.list.size()) {err = "IndexError: index out of range"; goto E1;}
          Atom item = a->val.list[b->val.i64];
          a->val.list.std::vector<Atom>::~vector();
          new (a) Atom(std::move(item));
        } else {err = "TypeError: bad value type for operator '[]'"; goto E1;}
        break;
      }

      case OpType::End:
        return;
      case OpType::Pop:
        (--stack_top)->~Atom();
        break;
        
      case OpType::Null:
        (stack_top++)->type = AtomType::Null;
        break;
      case OpType::Str:
        stack_top->type = AtomType::Str;
        new (&stack_top++->val.str) std::string(cmd.val.str);
        break;
      case OpType::I64:
        stack_top->type = AtomType::I64;
        (stack_top++)->val.i64 = cmd.val.i64;
        break;
      case OpType::F64:
        stack_top->type = AtomType::F64;
        (stack_top++)->val.f64 = cmd.val.f64;
        break;

      case OpType::Var:
        if (pvar[cmd.val.usize].type == AtomType::None) {err = "NameError: variable not found"; goto E2;}
        new (stack_top++) Atom(pvar[cmd.val.usize]);
        break;
      case OpType::Asg:
        pvar[cmd.val.usize] = stack_top[-1];
        break;
      case OpType::AsgPop:
        pvar[cmd.val.usize] = std::move(*--stack_top);
        stack_top->~Atom();
        break;

      case OpType::Call: {
        Atom *atom = stack_top - cmd.val.usize - 1;
        if (atom->type != AtomType::StdFn) {err = "TypeError: bad value type for function call"; goto E2;}
        atom->val.stdfn(cmd.val.usize, atom + 1);
        for (Atom *a = atom + 1; a != stack_top; a++) a->~Atom();
        stack_top = atom + 1;
        break;
      }
      case OpType::List: {
        std::vector<Atom> list;
        std::move(stack_top - cmd.val.usize, stack_top, std::back_inserter(list));
        // for (Atom *a = stack_top - cmd.val.usize; a != stack_top; a++) a->~Atom();
        stack_top = stack_top - cmd.val.usize;
        new (stack_top++) Atom(std::move(list));
        break;
      }
      case OpType::Jmp:
        p_op += cmd.val.size;
        goto L1;
      case OpType::Jift:
        --stack_top;
        if (stack_top->type == AtomType::I64) {
          if (stack_top->val.i64) {
            p_op += cmd.val.size;
            goto L1;
          }
          break;
        }
        if (atob(*stack_top)) {
          p_op += cmd.val.size;
          stack_top->~Atom();
          goto L1;
        }
        stack_top->~Atom();
        break;
      case OpType::Jiff:
        --stack_top;
        if (stack_top->type == AtomType::I64) {
          if (stack_top->val.i64) break;
          p_op += cmd.val.size;
          goto L1;
        }
        if (atob(*stack_top)) {
          stack_top->~Atom();
          break;
        }
        p_op += cmd.val.size;
        stack_top->~Atom();
        goto L1;
      case OpType::Land:
        --stack_top;
        if (stack_top->type == AtomType::I64 ? stack_top->val.i64 : atob(*stack_top)) {
          stack_top->~Atom();
          break;
        }
        stack_top++;
        p_op += cmd.val.size;
        goto L1;
      case OpType::Lor: {
        --stack_top;
        if (stack_top->type == AtomType::I64 ? stack_top->val.i64 : atob(*stack_top)) {
          stack_top->~Atom();
          p_op += cmd.val.size;
          goto L1;
        }
        stack_top->~Atom();
        break;
      }
    }
  }
  E1:
  stack_top->~Atom();
  E2:
  for (Atom *it = stack; it < stack_top; it++) it->~Atom();
  throw err;
}

#ifdef WIN32
#include <windows.h>
#endif
int main(int argc, char **argv) {
  bool color = 0;
  if (argc > 1) {
    char *filepath;
    if (argv[1][0] == 0) goto Help;
    if (argv[1][0] != '-') {filepath = argv[1]; goto RunFile;}
    if (argv[1][1] == 'h') goto Help;
    if (argv[1][1] == 'c') {color = 1; goto Repl;}
    if (argv[1][1] == 'r') {
      if (argc > 2) if (argv[2][0] == '-') if (argv[2][1] == 'c') color = 1;
      goto Repl;
    }
    if (argv[1][1] == 'f')
    if (argc > 2) {filepath = argv[2]; goto RunFile;}
    else goto Help;
    filepath = argv[1];
    goto RunFile;

    Help:
      std::cout << "Zemon Interpreter [v0.1.0]\nUsage: \n"
      << argv[0] << " -h            - help\n"
      << argv[0] << " [-r] [-c]     - repl [with color]\n"
      << argv[0] << " [-f] file.zns - run file\n";
      return 0;
    
    RunFile:
      std::ifstream file(filepath);
      if (!file.is_open()) {
        std::cerr << "! failed to open file\n";
        return 1;
      }
      try {
        std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        std::vector<Token> tokens = tokenize(str);
        std::vector<Op> ops;
        size_t stack_size = 0, stack_max_size = 0;
        expr_stmt(tokens, ops, stack_size, stack_max_size, true);
        ops.emplace_back(OpType::End);
        vars.resize(names.size());
        Atom *stack = (Atom *)malloc(stack_max_size * sizeof(Atom));
        eval(ops, stack);
        free(stack);
        return 0;
      }
      catch (std::string e) {
        std::cerr << "! " << e << "\n";
        return 2;
      }
  }
  Repl:
  #ifdef WIN32
  if (color) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  }
  #endif
  repl = 1;
  std::cout << "Zemon Interpreter [v0.1.0]\n";
  std::string input;
  while (1) {
    std::cout << (color ? "\033[1;33m> \033[0m" : "> ");
    std::getline(std::cin, input);
    try {
      L1:
      std::vector<Token> tokens = tokenize(input);
      if (tokens.empty()) continue;
      std::vector<Op> ops;
      size_t stack_size = 0, stack_max_size = 0;
      try {
        expr_stmt(tokens, ops, stack_size, stack_max_size, true);
      } catch (int) {
        std::string input2;
        std::cout << (color ? "\033[1;33m. \033[0m" : ". ");
        std::getline(std::cin, input2);
        input += "\n" + input2;
        goto L1;
      }
      ops.emplace_back(OpType::End);
      vars.resize(names.size());

      // for (Op& op : ops) std::cout << otos(op) << "\n";
      // std::cout << "stack_size: " << stack_size << "\n";
      // std::cout << "stack_max_size: " << stack_max_size << "\n";

      Atom *stack = stack = (Atom *)malloc(stack_max_size * sizeof(Atom));
      eval(ops, stack);
      if (stack_size) {
        std::cout << (color ? "\033[1;33m= " : "= ") << arepl(*stack) << "\n";
        stack->~Atom();
      }
      free(stack);
    } catch (std::string e) {
      std::cerr << (color ? "\033[1;31m! " : "! ") << e << "\n";
    }
  }
}
