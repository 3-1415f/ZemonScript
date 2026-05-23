#ifndef ZN_TOKENIZE_HPP
#define ZN_TOKENIZE_HPP

#include "includes.hpp"

std::vector<Token> tokenize(const std::string& input) {
  std::vector<Token> tokens;
  size_t i = 0;
  auto peek = [&]() -> char {
    return i < input.size() ? input[i] : '\0';
  };
  auto next = [&]() -> char {
    return i < input.size() ? input[i++] : '\0';
  };
  while (i < input.size()) {
    char c = next();
    if (std::isspace(c)) continue;
    if (c == '"' || c == '\'') {
      char quote = c;
      std::string s;
      while (true) {
        char nc;
        if (!(nc = next())) throw "SyntaxError: unterminated string";
        if (nc == '\\') {
          char esc = next();
          switch (esc) {
            case 'n': s += '\n'; break;
            case 't': s += '\t'; break;
            case 'r': s += '\r'; break;
            case '\\': s += '\\'; break;
            case '\"': s += '\"'; break;
            case '\'': s += '\''; break;
            default:
              throw "SyntaxError: bad escape sequence";
          }
        } else if (nc == quote)
          break;
        else
          s += nc;
      }
      tokens.emplace_back(Atom{s});
    } else if (isdigit(c)) {
      long long v = c - '0';
      double fv = v;
      double f = 1.0;
      bool dot = false;
      while (isdigit(c = peek()) || c == '.' || c == '_') {
        if (c == '_');
        else if (c == '.') {
          if (dot) throw "SyntaxError: multiple decimal points";
          fv = v, dot = true;
        }
        else if (dot) fv += (f *= 0.1) * (c-'0');
        else v = v*10 + c-'0';
        next();
      }
      if (dot) tokens.emplace_back(Atom{fv});
      else tokens.emplace_back(Atom{v});
    } else if (isalpha(c) || c == '_') {
      std::string s(1, c);
      while (isalnum(c = peek()) || c == '_') { s += c; next(); }
      if (s == "true") tokens.emplace_back(Atom{(long long)1});
      else if (s == "false") tokens.emplace_back(Atom{(long long)0});
      else if (s == "null") tokens.emplace_back(Atom{AtomType::Null});
      else if (s == "if") tokens.emplace_back(Symbol::If);
      else if (s == "elif") tokens.emplace_back(Symbol::Elif);
      else if (s == "else") tokens.emplace_back(Symbol::Else);
      else if (s == "while") tokens.emplace_back(Symbol::While);
      else tokens.emplace_back(s);
    } else switch (c) {
      case '/':
        if (peek() == '/')
          do next();
          while (peek() != '\n' && peek() != '\0');
        else tokens.emplace_back(Symbol::Div);
        break;

      case '+': tokens.emplace_back(Symbol::Add); break;
      case '-': tokens.emplace_back(Symbol::Sub); break;
      case '%': tokens.emplace_back(Symbol::Mod); break;
      case '~': tokens.emplace_back(Symbol::Bnot); break;
      case '^': tokens.emplace_back(Symbol::Bxor); break;
      case '(': tokens.emplace_back(Symbol::Lpar); break;
      case ')': tokens.emplace_back(Symbol::Rpar); break;
      case '[': tokens.emplace_back(Symbol::Lbrk); break;
      case ']': tokens.emplace_back(Symbol::Rbrk); break;
      case '{': tokens.emplace_back(Symbol::Lbkl); break;
      case '}': tokens.emplace_back(Symbol::Rbkl); break;
      case ',': tokens.emplace_back(Symbol::Sep); break;
      case ';': tokens.emplace_back(Symbol::Eos); break;
      case ':': tokens.emplace_back(Symbol::Cmn); break;
      case '?': tokens.emplace_back(Symbol::Qst); break;

      case '*': if (peek() == '*') {next(); tokens.emplace_back(Symbol::Pow);} else tokens.emplace_back(Symbol::Mul); break;
      case '&': if (peek() == '&') {next(); tokens.emplace_back(Symbol::Land);} else tokens.emplace_back(Symbol::Band); break;
      case '|': if (peek() == '|') {next(); tokens.emplace_back(Symbol::Lor);} else tokens.emplace_back(Symbol::Bor); break;
      case '=': if (peek() == '=') {next(); tokens.emplace_back(Symbol::Eq);} else  tokens.emplace_back(Symbol::Asg); break;
      case '!': if (peek() == '=') {next(); tokens.emplace_back(Symbol::Ne);} else tokens.emplace_back(Symbol::Lnot); break;
      case '<': if (peek() == '=') {next(); tokens.emplace_back(Symbol::Le);} else if (peek() == '<') {next(); tokens.emplace_back(Symbol::Shl);} else tokens.emplace_back(Symbol::Lt); break;
      case '>': if (peek() == '=') {next(); tokens.emplace_back(Symbol::Ge);} else if (peek() == '>') {next(); tokens.emplace_back(Symbol::Shr);} else tokens.emplace_back(Symbol::Gt); break;
      default:
        throw "SyntaxError: unknown character";
    }
  }
  std::reverse(tokens.begin(), tokens.end());
  return tokens;
}

#endif
