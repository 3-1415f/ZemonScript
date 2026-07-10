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

std::vector<std::string> names = {"exit", "out", "outln", "input", "bool", "int", "float", "str", "type", "repl", "clock", "sleep", "pi", "e", "sin", "cos", "tan", "asin", "acos", "atan"};

void expr(std::vector<Token>& tokens, std::vector<Op>& output, short expr_bp, size_t& stacksize, size_t& maxstacksize);

struct expr_stmt_args {
  std::vector<Token> tokens;
  std::vector<Op> output;
  size_t stacksize;
  size_t maxstacksize;
  std::vector<size_t> break_list;
  std::vector<size_t> continue_list;
};

void expr_stmt(expr_stmt_args& args, bool in_main = false) {
  if (!in_main) {
    if (peek(args.tokens).type == TokenType::Null)
      if (repl) throw 1;
      else throw "SyntaxError: statement not terminated, expect '{'";
    if (next(args.tokens).val.sym != Symbol::Lbkl) {
      throw "SyntaxError: bad operator, expect '{'";
    }
  }

  while (1) {
    if (peek(args.tokens).type == TokenType::Null)
      if (in_main) {
        if (!args.break_list.empty()) throw "SyntaxError: break not in loop";
        if (!args.continue_list.empty()) throw "SyntaxError: continue not in loop";
        return;
      }
      else if (repl) throw 1;
      else throw "SyntaxError: '{' not closed, expect '}'";

    if (peek(args.tokens).type != TokenType::Symbol) L2: {
      expr(args.tokens, args.output, 0, args.stacksize, args.maxstacksize);
      if (peek(args.tokens).type == TokenType::Null)
        if (in_main) {
          if (!args.break_list.empty()) throw "SyntaxError: break not in loop";
          if (!args.continue_list.empty()) throw "SyntaxError: continue not in loop";
          return;
        }
        else if (repl) throw 1;
        else throw "SyntaxError: '{' not closed, expect '}'";
      if (peek(args.tokens).val.sym == Symbol::Eos)
        while (peek(args.tokens).type == TokenType::Symbol && peek(args.tokens).val.sym == Symbol::Eos)
          next(args.tokens);
      else if (peek(args.tokens).val.sym != Symbol::Rbkl) throw "SyntaxError: bad operator";

      if (args.output.back().type == OpType::Asg) args.output.back().type = OpType::AsgPop;
      else args.output.emplace_back(OpType::Pop);
      args.stacksize--;
      continue;
    }

    switch (peek(args.tokens).val.sym) {
    case Symbol::Rbkl:
      if (in_main) throw "SyntaxError: bad operator";
      next(args.tokens);
      return;
    case Symbol::Lbkl:
      expr_stmt(args);
      break;

    case Symbol::Break:
      next(args.tokens);
      args.break_list.push_back(args.output.size());
      args.output.emplace_back(OpType::Jmp);
      break;
    case Symbol::Continue:
      next(args.tokens);
      args.continue_list.push_back(args.output.size());
      args.output.emplace_back(OpType::Jmp);
      break;

    case Symbol::Do: {
      next(args.tokens);
      size_t index = args.output.size();
      expr_stmt(args);
      for (size_t i : args.break_list)
        args.output[i].val.size = args.output.size() - i;
      args.break_list.clear();
      for (size_t i : args.continue_list)
        args.output[i].val.size = i - index;
      args.continue_list.clear();
      break;
    }

    case Symbol::If: {
      std::vector<size_t> jmps_to_end;
      L1:
      next(args.tokens);
      expr(args.tokens, args.output, 0, args.stacksize, args.maxstacksize);
      size_t jmp_index = args.output.size();
      args.output.emplace_back(OpType::Jiff);
      args.stacksize--;
      expr_stmt(args);
      if (peek(args.tokens).type == TokenType::Symbol)
      if (peek(args.tokens).val.sym == Symbol::Elif) {
        jmps_to_end.push_back(args.output.size());
        args.output.emplace_back(OpType::Jmp);
        args.output[jmp_index].val.size = args.output.size() - jmp_index;
        goto L1;
      } else if (peek(args.tokens).val.sym == Symbol::Else) {
        next(args.tokens);
        jmps_to_end.push_back(args.output.size());
        args.output.emplace_back(OpType::Jmp);
        args.output[jmp_index].val.size = args.output.size() - jmp_index;
        expr_stmt(args);
      } else args.output[jmp_index].val.size = args.output.size() - jmp_index;
      else args.output[jmp_index].val.size = args.output.size() - jmp_index;
      for (size_t i : jmps_to_end)
        args.output[i].val.size = args.output.size() - i;
    }
    break;

    case Symbol::While: {
      next(args.tokens);
      size_t cond_index = args.output.size();
      std::vector<Op> cond;
      expr(args.tokens, cond, 0, args.stacksize, args.maxstacksize);
      size_t body_index = args.output.size();
      args.output.emplace_back(OpType::Jmp);
      expr_stmt(args);
      args.output[body_index].val.size = args.output.size() - body_index;
      for (size_t i : args.continue_list)
        args.output[i].val.size = args.output.size() - i;
      args.continue_list.clear();
      args.output.insert(args.output.end(), cond.begin(), cond.end());
      args.output.emplace_back(OpType::Jift, cond_index - args.output.size() + 1);
      args.stacksize--;
      if (peek(args.tokens).type == TokenType::Symbol && peek(args.tokens).val.sym == Symbol::Else) {
        next(args.tokens);
        expr_stmt(args);
      }
      for (size_t i : args.break_list)
        args.output[i].val.size = args.output.size() - i;
      args.break_list.clear();
    }
    break;

    default: goto L2;
    }

    while (peek(args.tokens).type == TokenType::Symbol && peek(args.tokens).val.sym == Symbol::Eos) next(args.tokens);
  }
}

void expr(std::vector<Token>& tokens, std::vector<Op>& output, short min_bp, size_t& stacksize, size_t& maxstacksize) {
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
        case AtomType::Str: {
          output.emplace_back(token.val.atom.val.str);
          break;
        }
      }
      if (maxstacksize < ++stacksize) maxstacksize = stacksize;
      break;

    case TokenType::Symbol:
      switch (token.val.sym) {
        case Symbol::Lpar:
          expr(tokens, output, 0, stacksize, maxstacksize);
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
            expr(tokens, output, 0, stacksize, maxstacksize);
            count++;
            Token token = next(tokens);
            if (token.type == TokenType::Null)
              if (repl) throw 1;
              else throw "SyntaxError: '[' not closed, expect ']'";
            if (token.val.sym == Symbol::Rbrk) break;
            if (token.val.sym != Symbol::Sep) throw "SyntaxError: bad operator";
          }
          output.emplace_back(OpType::List, count);
          if (maxstacksize < (stacksize += 1-count)) maxstacksize = stacksize;
          break;
        }

        default:
          short bp = prefix(token.val.sym);
          if (!bp) throw "SyntaxError: bad operator, expect prefix operator";
          if (token.val.sym == Symbol::Add || token.val.sym == Symbol::Sub) {
            output.emplace_back(0ll);
            if (maxstacksize < ++stacksize) maxstacksize = stacksize;
          }
          expr(tokens, output, bp&255, stacksize, maxstacksize);
          output.emplace_back((OpType)(char)token.val.sym);
          stacksize--;
      }
      break;

    case TokenType::Id:
      std::vector<std::string>::iterator it = std::find(names.begin(), names.end(), token.val.id);
      size_t index;
      if (it != names.end())
        index = it - names.begin();
      else {
        index = names.size();
        names.emplace_back(token.val.id);
      }
      if (peek(tokens).type == TokenType::Symbol && peek(tokens).val.sym == Symbol::Asg) {
        next(tokens);
        expr(tokens, output, 0, stacksize, maxstacksize);
        output.emplace_back(OpType::Asg, index);
      } else {
        output.emplace_back(OpType::Var, index);
        if (maxstacksize < ++stacksize) maxstacksize = stacksize;
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
          expr(tokens, output, 0, stacksize, maxstacksize);
          count++;
          Token token = next(tokens);
          if (token.type == TokenType::Null)
            if (repl) throw 1;
            else throw "SyntaxError: '(' not closed, expect ')'";
          if (token.val.sym == Symbol::Rpar) break;
          if (token.val.sym != Symbol::Sep) throw "SyntaxError: bad operator";
        }
        output.emplace_back(OpType::Call, count);
        stacksize -= count;
        break;
      }
      case Symbol::Lbrk:
        next(tokens);
        expr(tokens, output, 0, stacksize, maxstacksize);
        if (peek(tokens).type == TokenType::Null)
          if (repl) throw 1;
          else throw "SyntaxError: '[' not closed, expect ']'";
        if (next(tokens).val.sym != Symbol::Rbrk) throw "SyntaxError: bad operator";
        // TODO: AsgInd
        output.emplace_back(OpType::Ind);
        stacksize--;
        break;
      case Symbol::Qst: {
        if (0 < min_bp) return;
        next(tokens);
        size_t index = output.size();
        output.emplace_back(OpType::Jiff);
        stacksize--;
        expr(tokens, output, 0, stacksize, maxstacksize);
        if (peek(tokens).type == TokenType::Null)
          if (repl) throw 1;
          else throw "SyntaxError: '?' not closed, expect ':'";
        size_t index2 = output.size();
        output.emplace_back(OpType::Jmp);
        stacksize--;
        output[index].val.size = output.size() - index;
        if (next(tokens).val.sym != Symbol::Cmn) throw "SyntaxError: bad operator";
        expr(tokens, output, 0, stacksize, maxstacksize);
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
        stacksize--;
        expr(tokens, output, bp&255, stacksize, maxstacksize);
        output[index].val.size = output.size() - index;
        continue;
      }
      expr(tokens, output, bp&255, stacksize, maxstacksize);
      output.emplace_back((OpType)(char)token.val.sym);
      stacksize--;
    }
  }
}

std::vector<Atom> vars = {f_exit, f_out, f_outln, f_input, f_bool, f_int, f_float, f_str, f_type, f_repl, f_clock, f_sleep, 3.1415926, 2.7182818, f_sin, f_cos, f_tan, f_asin, f_acos, f_atan};

void eval(const std::vector<Op>& ops, Atom *stack) {
  Atom *pvar = &vars[0];
  Atom *top = stack;
  const char *err;
  const Op *pcmd = &ops[0];

  while (1) {
    Op cmd = *pcmd;
    switch (cmd.type) {
      case OpType::Pop:
        (--top)->~Atom();
        break;

      case OpType::Null:
        top->type = AtomType::Null;
        top++;
        break;
      case OpType::I64:
        top->type = AtomType::I64;
        top->val.i64 = cmd.val.i64;
        top++;
        break;
      case OpType::F64:
        top->type = AtomType::F64;
        top->val.f64 = cmd.val.f64;
        top++;
        break;
      case OpType::Str:
        top->type = AtomType::Str;
        new (&top->val.str) std::string(cmd.val.str);
        top++;
        break;

      case OpType::Var:
        if (pvar[cmd.val.usize].type == AtomType::None) {err = "NameError: variable not found"; goto E2;}
        new (top) Atom(pvar[cmd.val.usize]);
        top++;
        break;
      case OpType::Asg:
        pvar[cmd.val.usize] = top[-1];
        break;
      case OpType::AsgPop:
        pvar[cmd.val.usize] = std::move(*--top);
        top->~Atom();
        break;

      case OpType::Call: {
        Atom *atom = top - cmd.val.usize;
        if (atom[-1].type != AtomType::StdFn) {err = "TypeError: bad value type for function call"; goto E2;}
        atom[-1].val.stdfn(cmd.val.usize, atom);
        for (Atom *a = atom; a != top; a++) a->~Atom();
        top = atom;
        break;
      }
      case OpType::List: {
        std::vector<Atom> list;
        std::move(top - cmd.val.usize, top, std::back_inserter(list));
        for (Atom *a = top - cmd.val.usize; a != top; a++) a->~Atom();
        top = top - cmd.val.usize;
        new (top) Atom(std::move(list));
        top++;
        break;
      }
      case OpType::Jmp:
        pcmd += cmd.val.size;
        continue;
      case OpType::Jift:
        --top;
        if (top->type == AtomType::I64) {
          if (top->val.i64) {
            pcmd += cmd.val.size;
            continue;
          }
          break;
        }
        if (atob(*top)) {
          top->~Atom();
          pcmd += cmd.val.size;
          continue;
        }
        top->~Atom();
        break;
      case OpType::Jiff:
        --top;
        if (top->type == AtomType::I64) {
          if (top->val.i64) break;
          pcmd += cmd.val.size;
          continue;
        }
        if (atob(*top)) {
          top->~Atom();
          break;
        }
        pcmd += cmd.val.size;
        top->~Atom();
        continue;

      case OpType::Lnot: {
        Atom& a = top[-1];
        switch (a.type) {
          case AtomType::Null:
            a.type = AtomType::I64;
            a.val.i64 = 1;
            break;
          case AtomType::I64:
            a.val.i64 = !a.val.i64;
            break;
          case AtomType::F64:
            a.type = AtomType::I64;
            a.val.i64 = !a.val.f64;
            break;
          case AtomType::Str: {
            bool b = a.val.str.empty();
            a.val.str.std::string::~string();
            a.type = AtomType::I64;
            a.val.i64 = b;
            break;
          }
          case AtomType::List: {
            bool b = a.val.list.empty();
            a.val.list.std::vector<Atom>::~vector();
            a.type = AtomType::I64;
            a.val.i64 = b;
            break;
          }
          case AtomType::StdFn:
            a.type = AtomType::I64;
            a.val.i64 = 1;
            break;
          default: {err = "TypeError: bad value type for operator '!'"; goto E2;}
        }
        break;
      }
      case OpType::Bnot: {
        Atom& a = top[-1];
        if (a.type == AtomType::I64) a.val.i64 = ~a.val.i64;
        else {err = "TypeError: bad value type for operator '~'"; goto E2;}
        break;
      }

      case OpType::Pow: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = pow(a.val.i64, b.val.i64);
          else if (b.type == AtomType::F64)
            a.type = AtomType::F64,
            a.val.f64 = pow(a.val.i64, b.val.f64);
          else {err = "TypeError: bad value type for operator '**'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.f64 = pow(a.val.f64, b.val.i64);
          else if (b.type == AtomType::F64)
            a.val.f64 = pow(a.val.f64, b.val.f64);
          else {err = "TypeError: bad value type for operator '**'"; goto E1;}
        else {err = "TypeError: bad value type for operator '**'"; goto E1;}
        break;
      }
      case OpType::Mul: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 *= b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::F64,
            a.val.f64 = a.val.i64 * b.val.f64;
          else {err = "TypeError: bad value type for operator '*'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.f64 *= b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.f64 *= b.val.f64;
          else {err = "TypeError: bad value type for operator '*'"; goto E1;}
        else if (a.type == AtomType::Str && b.type == AtomType::I64) {
          if (b.val.i64 < 0) {err = "ArgError: bad argument range for operator '*'"; goto E2;}
          if (b.val.i64 == 0) {a.val.str = ""; break;}
          std::string s = a.val.str;
          while (--b.val.i64) a.val.str += s;
        }
        else {err = "TypeError: bad value type for operator '*'"; goto E1;}
        break;
      }
      case OpType::Div: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 /= b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::F64,
            a.val.f64 = a.val.i64 / b.val.f64;
          else {err = "TypeError: bad value type for operator '/'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.f64 /= b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.f64 /= b.val.f64;
          else {err = "TypeError: bad value type for operator '/'"; goto E1;}
        break;
      }
      case OpType::Mod: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 % b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::F64,
            a.val.f64 = fmod(a.val.i64, b.val.f64);
          else {err = "TypeError: bad value type for operator '%'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.f64 = fmod(a.val.f64, b.val.i64);
          else if (b.type == AtomType::F64)
            a.val.f64 = fmod(a.val.f64, b.val.f64);
          else {err = "TypeError: bad value type for operator '%'"; goto E1;}
        break;
      }

      case OpType::Add: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 += b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::F64,
            a.val.f64 = a.val.i64 + b.val.f64;
          else {err = "TypeError: bad value type for operator '+'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.f64 += b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.f64 += b.val.f64;
          else {err = "TypeError: bad value type for operator '+'"; goto E1;}
        else if (a.type == AtomType::Str && b.type == AtomType::Str) {
          a.val.str += b.val.str;
          b.val.str.std::string::~string();
        }
        else if (a.type == AtomType::List && b.type == AtomType::List) {
          a.val.list.insert(a.val.list.end(), std::make_move_iterator(b.val.list.begin()), std::make_move_iterator(b.val.list.end()));
          b.val.list.std::vector<Atom>::~vector();
        }
        else throw "TypeError: bad value type for operator '+'";
        break;
      }
      case OpType::Sub: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 -= b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::F64,
            a.val.f64 = a.val.i64 - b.val.f64;
          else {err = "TypeError: bad value type for operator '-'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.f64 -= b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.f64 -= b.val.f64;
          else {err = "TypeError: bad value type for operator '-'"; goto E1;}
        else throw "TypeError: bad value type for operator '-'";
        break;
      }

      case OpType::Shl: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64 && b.type == AtomType::I64)
          a.val.i64 <<= b.val.i64;
        else {err = "TypeError: bad value type for operator '<<'"; goto E1;}
        break;
      }
      case OpType::Shr: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64 && b.type == AtomType::I64)
          a.val.i64 >>= b.val.i64;
        else {err = "TypeError: bad value type for operator '>>'"; goto E1;}
        break;
      }

      case OpType::Lt: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 < b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.i64 = a.val.i64 < b.val.f64;
          else {err = "TypeError: bad value type for operator '<'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.f64 < b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 < b.val.f64;
          else {err = "TypeError: bad value type for operator '<'"; goto E1;}
        else throw "TypeError: bad value type for operator '<'";
        break;
      }
      case OpType::Le: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 <= b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.i64 = a.val.i64 <= b.val.f64;
          else {err = "TypeError: bad value type for operator '<='"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 <= b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 <= b.val.f64;
          else {err = "TypeError: bad value type for operator '<='"; goto E1;}
        else {err = "TypeError: bad value type for operator '<='"; goto E1;}
        break;
      }
      case OpType::Gt: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 > b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.i64 = a.val.i64 > b.val.f64;
          else {err = "TypeError: bad value type for operator '>'"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.f64 > b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 > b.val.f64;
          else {err = "TypeError: bad value type for operator '>'"; goto E1;}
        else {err = "TypeError: bad value type for operator '>'"; goto E1;}
        break;
      }
      case OpType::Ge: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 >= b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.i64 = a.val.i64 >= b.val.f64;
          else {err = "TypeError: bad value type for operator '>='"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 >= b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 >= b.val.f64;
          else {err = "TypeError: bad value type for operator '>='"; goto E1;}
        else {err = "TypeError: bad value type for operator '>='"; goto E1;}
        break;
      }
      case OpType::Ne: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 != b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.i64 = a.val.i64 != b.val.f64;
          else {err = "TypeError: bad value type for operator '!='"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 != b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 != b.val.f64;
          else {err = "TypeError: bad value type for operator '!='"; goto E1;}
        else {err = "TypeError: bad value type for operator '!='"; goto E1;}
        break;
      }
      case OpType::Eq: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64)
          if (b.type == AtomType::I64)
            a.val.i64 = a.val.i64 == b.val.i64;
          else if (b.type == AtomType::F64)
            a.val.i64 = a.val.i64 == b.val.f64;
          else {err = "TypeError: bad value type for operator '=='"; goto E1;}
        else if (a.type == AtomType::F64)
          if (b.type == AtomType::I64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 == b.val.i64;
          else if (b.type == AtomType::F64)
            a.type = AtomType::I64,
            a.val.i64 = a.val.f64 == b.val.f64;
          else {err = "TypeError: bad value type for operator '=='"; goto E1;}
        else {err = "TypeError: bad value type for operator '=='"; goto E1;}
        break;
      }

      case OpType::Band: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64 && b.type == AtomType::I64)
          a.val.i64 &= b.val.i64;
        else if (a.type == AtomType::List) {
          a.val.list.push_back(std::move(b));
          b.~Atom();
        }
        else {err = "TypeError: bad value type for operator '&'"; goto E1;}
        break;
      }
      case OpType::Bxor: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64 && b.type == AtomType::I64)
          a.val.i64 ^= b.val.i64;
        else {err = "TypeError: bad value type for operator '^'"; goto E1;}
        break;
      }
      case OpType::Bor: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::I64 && b.type == AtomType::I64)
          a.val.i64 |= b.val.i64;
        else {err = "TypeError: bad value type for operator '|'"; goto E1;}
        break;
      }

      case OpType::Ind: {
        Atom& b = *(--top);
        Atom& a = *(top-1);
        if (a.type == AtomType::Str && b.type == AtomType::I64) {
          if (b.val.i64 >= a.val.str.size()) {err = "IndexError: index out of range"; goto E1;}
          a.val.str = a.val.str[b.val.i64];
        } else if (a.type == AtomType::List && b.type == AtomType::I64) {
          if (b.val.i64 >= a.val.list.size()) {err = "IndexError: index out of range"; goto E1;}
          Atom item = a.val.list[b.val.i64];
          a.val.list.std::vector<Atom>::~vector();
          new (&a) Atom(std::move(item));
        } else {err = "TypeError: bad value type for operator '[]'"; goto E1;}
        break;
      }

      case OpType::Land:
        --top;
        if (top->type == AtomType::I64 ? top->val.i64 : atob(*top)) {
          top->~Atom();
          break;
        }
        top++;
        pcmd += cmd.val.size;
        continue;
      case OpType::Lor: {
        --top;
        if (top->type == AtomType::I64 ? top->val.i64 : atob(*top)) {
          top->~Atom();
          pcmd += cmd.val.size;
          continue;
        }
        top->~Atom();
        break;
      }

      case OpType::End:
        return;
    }
    pcmd++;
  }
  E1:
  top->~Atom();
  E2:
  for (Atom *it = stack; it < top; it++) it->~Atom();
  throw err;
}

#ifdef ZN_DEBUG
std::string otos(Op op) {
  switch (op.type) {
    case OpType::Bnot: return "~";
    case OpType::Lnot: return "!";
    case OpType::Add: return "+";
    case OpType::Sub: return "-";
    case OpType::Mul: return "*";
    case OpType::Div: return "/";
    case OpType::Mod: return "%";
    case OpType::Pow: return "**";
    case OpType::Shl: return "<<";
    case OpType::Shr: return ">>";
    case OpType::Lt: return "<";
    case OpType::Le: return "<=";
    case OpType::Gt: return ">";
    case OpType::Ge: return ">=";
    case OpType::Ne: return "!=";
    case OpType::Eq: return "==";
    case OpType::Band: return "&";
    case OpType::Bxor: return "^";
    case OpType::Bor: return "|";
    case OpType::Ind: return "[]";
    case OpType::AsgInd: return "[]=";
    case OpType::End: return "END";
    case OpType::Pop: return "POP";
    case OpType::Null: return "null";
    case OpType::Str: return const_str[op.val.usize];
    case OpType::I64: return std::to_string(op.val.i64);
    case OpType::F64: return ftos(op.val.f64);
    case OpType::Var: return names[op.val.usize];
    case OpType::Asg: return "=" + names[op.val.usize];
    case OpType::AsgPop: return "=" + names[op.val.usize] + "&POP";
    case OpType::Call: return "CALL," + std::to_string(op.val.usize);
    case OpType::List: return "LIST," + std::to_string(op.val.usize);
    case OpType::Jmp: return "JMP," + std::to_string(op.val.size);
    case OpType::Jift: return "JIFT," + std::to_string(op.val.size);
    case OpType::Jiff: return "JIFF," + std::to_string(op.val.size);
    case OpType::Land: return "&&," + std::to_string(op.val.size);
    case OpType::Lor: return "||," + std::to_string(op.val.size);
  }
  return "";
}
#endif

int main(int argc, char **argv) {
  char *filepath;
  if (argc > 1) {
    if (argv[1][0] == '-') if (argv[1][1] == 'h') {
      std::cout << "Zemon Interpreter [v1.0]\nUsage: \n"
      << argv[0] << "          - repl\n"
      << argv[0] << " -h       - help\n"
      << argv[0] << " file.zns - run file\n";
      return 0;
    }

    filepath = argv[1];
    std::ifstream file(filepath);
    if (!file.is_open()) {
      std::cerr << "! failed to open file\n";
      return 1;
    }
    try {
      std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      expr_stmt_args args = {lex(str), std::vector<Op>(), 0, 0, std::vector<size_t>(), std::vector<size_t>()};
      expr_stmt(args, true);

      args.output.emplace_back(OpType::End);
      vars.resize(names.size());
      Atom *stack = (Atom *)malloc(args.maxstacksize * sizeof(Atom));
      eval(args.output, stack);
      free(stack);
      return 0;
    }
    catch (const char *e) {
      std::cerr << "! " << e << "\n";
      return 2;
    }
  }

  repl = 1;
  std::cout << "Zemon Interpreter [v1.0]\n";
  std::string input;
  while (1) {
    std::cout << "> ";
    std::getline(std::cin, input);
    expr_stmt_args args;
    try {
      L1:
      args.tokens = lex(input);
      if (args.tokens.empty()) continue;
      args.output.clear();
      args.stacksize = 0;
      args.maxstacksize = 0;
      args.continue_list.clear();
      args.break_list.clear();
      try {
        expr_stmt(args, true);
      } catch (int) {
        std::string input2;
        std::cout << ". ";
        std::getline(std::cin, input2);
        input += "\n" + input2;
        goto L1;
      }

      args.output.emplace_back(OpType::End);
      vars.resize(names.size());

      #ifdef ZN_DEBUG
      for (Op& op : args.output) std::cout << otos(op) << "\n";
      #endif

      Atom *stack = (Atom *)malloc(args.maxstacksize * sizeof(Atom));
      eval(args.output, stack);
      if (args.stacksize) {
        std::cout << "= " << arepl(*stack) << "\n";
        stack->~Atom();
      }
      free(stack);
    } catch (const char *e) {
      std::cerr << "! " << e << "\n";
    }
  }
}