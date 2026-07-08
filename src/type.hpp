#ifndef ZN_TYPE_HPP
#define ZN_TYPE_HPP

#include "includes.hpp"

std::string ftos(double v) {
  if (std::isnan(v)) return "nan";
  if (std::isinf(v)) return (v < 0) ? "-inf" : "inf";

  std::ostringstream oss;
  oss << std::defaultfloat << std::setprecision(15) << v;
  std::string s = oss.str();
  return s.find('.') == std::string::npos ? s + ".0" : s;
}

inline bool atob(Atom& atom) {
  switch (atom.type) {
    case AtomType::Null: return false;
    case AtomType::I64: return atom.val.i64;
    case AtomType::F64: return atom.val.f64;
    case AtomType::Str: return !atom.val.str.empty();
    case AtomType::List: return !atom.val.list.empty();
    case AtomType::StdFn: return true;
    default: throw "TypeError: bad value type for bool()";
  }
}

long long atoi(Atom& atom) {
  switch (atom.type) {
    case AtomType::I64: return atom.val.i64;
    case AtomType::F64: return (long long)atom.val.f64;
    case AtomType::Str: return stoll(atom.val.str);
    default: throw "TypeError: bad value type for int()";
  }
}

std::string arepl(Atom atom) {
  switch (atom.type) {
    case AtomType::Null: return "null";
    case AtomType::I64: return std::to_string(atom.val.i64);
    case AtomType::F64: return ftos(atom.val.f64);
    case AtomType::Str: {
      std::string result = "'";
      for (char c : atom.val.str)
        switch (c) {
          case '\\': result += "\\\\"; break;
          case '\'': result += "\\'"; break;
          case '\a': result += "\\a"; break;
          case '\b': result += "\\b"; break;
          case '\t': result += "\\t"; break;
          case '\n': result += "\\n"; break;
          case '\v': result += "\\v"; break;
          case '\f': result += "\\f"; break;
          case '\r': result += "\\r"; break;
          default:
          if (isprint(c)) {
            result += c;
            break;
          }
          const char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
          result += std::string("\\x") + hex[(c >> 4) & 0xf] + hex[c & 0xf];
        }
      return result + "'";
    }
    case AtomType::List: {
      std::string result = "[";
      for (const auto& a : atom.val.list)
        result += arepl(a) + ", ";
      if (!atom.val.list.empty())
        result.resize(result.size() - 2);
      result += "]";
      return result;
    }
    case AtomType::StdFn: {
      std::ostringstream oss;
      oss << "<zn-std-function @ 0x" << std::hex << std::setw(sizeof(size_t) * 2) << std::setfill('0') << (size_t)atom.val.stdfn << std::dec << ">";
      return oss.str();
    }
  }
  return "";
}

std::string atos(Atom atom) {
  switch (atom.type) {
    case AtomType::Null: return "null";
    case AtomType::I64: return std::to_string(atom.val.i64);
    case AtomType::F64: return ftos(atom.val.f64);
    case AtomType::Str: return atom.val.str;
    case AtomType::List: {
      std::string result = "[";
      for (const auto& a : atom.val.list)
        result += arepl(a) + ", ";
      if (!atom.val.list.empty())
        result.resize(result.size() - 2);
      result += "]";
      return result;
    }
    case AtomType::StdFn: {
      std::ostringstream oss;
      oss << "<zn-std-function @ 0x" << std::hex << std::setw(sizeof(size_t) * 2) << std::setfill('0') << (size_t)atom.val.stdfn << std::dec << ">";
      return oss.str();
    }
  }
  return "";
}

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
    case OpType::Str: return op.val.str;
    case OpType::I64: return std::to_string(op.val.i64);
    case OpType::F64: return ftos(op.val.f64);
    case OpType::Var: return '#' + std::to_string(op.val.usize);
    case OpType::Asg: return "=#" + std::to_string(op.val.usize);
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