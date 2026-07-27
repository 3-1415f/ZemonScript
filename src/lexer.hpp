#ifndef ZN_TOKENIZE_HPP
#define ZN_TOKENIZE_HPP

#include "includes.hpp"

std::vector<Token> lex(const std::string& input) {
  std::vector<Token> tokens;
  const size_t len = input.size();
  size_t i = 0;
  while (i < len) {
    char c = input[i++];
    if (isspace(c)) continue;
    if (c == '"' || c == '\'') {
      char quote = c;
      std::string s;
      while (true) {
        if (i >= len) throw "SyntaxError: unterminated string";
        c = input[i++];
        if (c == quote) break;
        if (c == '\\') {
          if (i >= len) throw "SyntaxError: unterminated string";
          switch (c = input[i++]) {
            case 'a': s += '\a'; break;
            case 'b': s += '\b'; break;
            case 't': s += '\t'; break;
            case 'n': s += '\n'; break;
            case 'v': s += '\v'; break;
            case 'f': s += '\f'; break;
            case 'r': s += '\r'; break;
            case '"': s += '"'; break;
            case '\'': s += '\''; break;
            case '\\': s += '\\'; break;
            case 'x': {
              if (i+2 >= len) throw "SyntaxError: unterminated string";
              char ch = 0;
              c = input[i++];
              if (c >= '0' && c <= '9') ch = (c - '0') << 4;
              else if (c >= 'a' && c <= 'f') ch = (c - 'a') << 4;
              else if (c >= 'A' && c <= 'F') ch = (c - 'A') << 4;
              else throw "SyntaxError: bad escape sequence";
              c = input[i++];
              if (c >= '0' && c <= '9') s += ch | (c - '0');
              else if (c >= 'a' && c <= 'f') s += ch | (c - 'a');
              else if (c >= 'A' && c <= 'F') s += ch | (c - 'A');
              else throw "SyntaxError: bad escape sequence";
              break;
            }
            default:
              if (c < '0' || c > '7') throw "SyntaxError: bad escape sequence";
              if (i+2 >= len) throw "SyntaxError: unterminated string";
              char ch = (c - '0') << 6;
              c = input[i++];
              if (c < '0' || c > '7') throw "SyntaxError: bad escape sequence";
              ch |= (c - '0') << 3;
              c = input[i++];
              if (c < '0' || c > '7') throw "SyntaxError: bad escape sequence";
              s += ch | (c - '0');
              break;
          }
        }
        else s += c;
      }
      tokens.emplace_back(Atom{s});
    } else if (c >= '0' && c <= '9') {
      long long v = c - '0';
      double fv = c - '0', f = 1.0;
      bool dot = false;
      while (i < len) {
        c = input[i];
        if (c == '.')
        if (dot) throw "SyntaxError: multiple decimal points";
        else dot = true, i++;
        else if (c == '_') i++;
        else if (c >= '0' && c <= '9')
        if (dot) fv += (f *= 0.1) * (c-'0'), i++;
        else v = v*10 + c-'0', fv = fv*10 + c-'0', i++;
        else break;
      }
      if (dot) tokens.emplace_back(Atom{fv});
      else tokens.emplace_back(Atom{v});
    } else if (isalpha(c) || c == '_') {
      std::string s(1, c);
      while (i < len) {
        c = input[i];
        if (!isalnum(c) && c != '_') break;
        s += c, i++;
      }
      if (s == "null") tokens.emplace_back(Atom{AtomType::Null});
      else if (s == "true") tokens.emplace_back(Atom{1ll});
      else if (s == "false") tokens.emplace_back(Atom{0ll});
      else if (s == "do") tokens.emplace_back(Symbol::Do);
      else if (s == "if") tokens.emplace_back(Symbol::If);
      else if (s == "elif") tokens.emplace_back(Symbol::Elif);
      else if (s == "else") tokens.emplace_back(Symbol::Else);
      else if (s == "while") tokens.emplace_back(Symbol::While);
      else if (s == "break") tokens.emplace_back(Symbol::Break);
      else if (s == "continue") tokens.emplace_back(Symbol::Continue);
      else if (s == "fn") tokens.emplace_back(Symbol::Fn);
      else if (s == "return") tokens.emplace_back(Symbol::Return);
      else if (s == "local") tokens.emplace_back(Symbol::Local);
      else tokens.emplace_back(s);
    } else switch (c) {
      case '/':
        if (i < len ? input[i] == '/' : 0)
          do i++;
          while (i < len ? input[i] != '\n' : 0);
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

      case '&': if (i < len ? input[i] == '&' : 0) {i++; tokens.emplace_back(Symbol::Land);} else tokens.emplace_back(Symbol::Band); break;
      case '*': if (i < len ? input[i] == '*' : 0) {i++; tokens.emplace_back(Symbol::Pow);} else tokens.emplace_back(Symbol::Mul); break;
      case '|': if (i < len ? input[i] == '|' : 0) {i++; tokens.emplace_back(Symbol::Lor);} else tokens.emplace_back(Symbol::Bor); break;
      case '=': if (i < len ? input[i] == '=' : 0) {i++; tokens.emplace_back(Symbol::Eq);} else  tokens.emplace_back(Symbol::Asg); break;
      case '!': if (i < len ? input[i] == '=' : 0) {i++; tokens.emplace_back(Symbol::Ne);} else tokens.emplace_back(Symbol::Lnot); break;
      case '<': if (i < len ? input[i] == '=' : 0) {i++; tokens.emplace_back(Symbol::Le);} else if (i < len ? input[i] == '<' : 0) {i++; tokens.emplace_back(Symbol::Shl);} else tokens.emplace_back(Symbol::Lt); break;
      case '>': if (i < len ? input[i] == '=' : 0) {i++; tokens.emplace_back(Symbol::Ge);} else if (i < len ? input[i] == '>' : 0) {i++; tokens.emplace_back(Symbol::Shr);} else tokens.emplace_back(Symbol::Gt); break;
      default:
        throw "SyntaxError: unknown character";
    }
  }
  std::reverse(tokens.begin(), tokens.end());
  return tokens;
}

#endif
