#ifndef TYPE_HPP
#define TYPE_HPP
#include "includes.hpp"

inline bool tobool(Atom& atom) {
  switch (atom.type) {
    case AtomType::Null: return false;
    case AtomType::I64: return atom.val.i64;
    case AtomType::F64: return atom.val.f64;
    case AtomType::Str: return !atom.val.str.empty();
    case AtomType::List: return !atom.val.list.empty();
    case AtomType::Bltfn: return true;
    default: throw "TypeError: bad value type for bool()";
  }
}

long long toint(Atom& atom) {
  switch (atom.type) {
    case AtomType::I64: return atom.val.i64;
    case AtomType::F64: return (long long)atom.val.f64;
    case AtomType::Str: return stoll(atom.val.str);
    default: throw "TypeError: bad value type for int()";
  }
}

std::string tostr(Atom atom) {
  switch (atom.type) {
    case AtomType::Null: return "null";
    case AtomType::I64: return std::to_string(atom.val.i64);
    case AtomType::F64: return std::to_string(atom.val.f64);
    case AtomType::Str: return atom.val.str;
    case AtomType::List: {
      std::string result = "[";
      for (const auto& a : atom.val.list)
        result += tostr(a) + ", ";
      if (!atom.val.list.empty())
        result.resize(result.size() - 2);
      result += "]";
      return result;
    }
    case AtomType::Bltfn: return "<builtin-function>";
  }
  throw "FmtError: BadAtomToFormat";
}

#endif