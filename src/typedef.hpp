#ifndef ZN_TYPEDEF_HPP
#define ZN_TYPEDEF_HPP

#include "includes.hpp"

enum class Symbol : char {
  Lnot, Bnot,
  Pow,
  Mul, Div, Mod,
  Add, Sub,
  Shl, Shr,
  Lt, Le, Gt, Ge,
  Ne, Eq,
  Band, Bxor, Bor, Land, Lor,
  Lpar, Rpar, Lbrk, Rbrk, Lbkl, Rbkl,
  Sep, Eos, Asg, Cmn, Qst,
  If, Elif, Else, While,
};

enum class OpType : char {
  Lnot, Bnot,
  Pow, Mul, Div, Mod, Add, Sub,
  Shl, Shr,
  Lt, Le, Gt, Ge, Ne, Eq,
  Band, Bxor, Bor,
  Ind, AsgInd,
  End, Pop,
  Atom,
  Var, Asg, Call, List, Jmp, Jift, Jiff, Land, Lor
};

enum class AtomType : char  { None, Null, I64, F64, Str, List, Bltfn };
typedef struct Atom Atom;
typedef void (*BltFn)(int, Atom*);

typedef struct Atom {
  AtomType type;
  union Val {
    long long i64;
    double f64;
    std::string str;
    std::vector<Atom> list;
    BltFn bltfn;
    Val() {}
    ~Val() {}
  } val;

  Atom(const Atom& a) : type(a.type) {
    if (a.type == AtomType::I64)
      val.i64 = a.val.i64;
    else if (a.type == AtomType::F64)
      val.f64 = a.val.f64;
    else if (a.type == AtomType::Str)
      new (&val.str) std::string(a.val.str);
    else if (a.type == AtomType::List)
      new (&val.list) std::vector<Atom>(a.val.list);
    else if (a.type == AtomType::Bltfn)
      val.bltfn = a.val.bltfn;
    else type = AtomType::Null;
  }

  Atom(Atom&& a) : type(a.type) {
    if (a.type == AtomType::I64)
      val.i64 = a.val.i64;
    else if (a.type == AtomType::F64)
      val.f64 = a.val.f64;
    else if (a.type == AtomType::Str)
      new (&val.str) std::string(std::move(a.val.str));
    else if (a.type == AtomType::List)
      new (&val.list) std::vector<Atom>(std::move(a.val.list));
    else if (a.type == AtomType::Bltfn)
      val.bltfn = std::move(a.val.bltfn);
    else type = AtomType::Null;
  }

  Atom& operator=(const Atom& a) {
    if (this == &a) return *this;

    if (type == AtomType::Str)
      val.str.std::string::~string();
    else if (type == AtomType::List)
      val.list.std::vector<Atom>::~vector();

    type = a.type;
    if (type == AtomType::I64)
      val.i64 = a.val.i64;
    else if (type == AtomType::F64)
      val.f64 = a.val.f64;
    else if (type == AtomType::Str)
      new (&val.str) std::string(a.val.str);
    else if (type == AtomType::List)
      new (&val.list) std::vector<Atom>(a.val.list);
    else if (type == AtomType::Bltfn)
      val.bltfn = a.val.bltfn;
    else type = AtomType::Null;

    return *this;
  }

  ~Atom() {
    if (type == AtomType::Str)
      val.str.std::string::~string();
    else if (type == AtomType::List)
      val.list.std::vector<Atom>::~vector();
  }

  Atom() : type(AtomType::None) {val.i64 = 0;}
  Atom(AtomType t) : type(t) {val.i64 = 0;}
  Atom(double v) : type(AtomType::F64) { val.f64 = v; }
  Atom(long long v) : type(AtomType::I64) { val.i64 = v; }
  Atom(const std::string& v) : type(AtomType::Str) { new (&val.str) std::string(v); }
  Atom(const std::vector<Atom>& v) : type(AtomType::List) { new (&val.list) std::vector<Atom>(v); }
  Atom(BltFn v) : type(AtomType::Bltfn) { val.bltfn = v; }
} Atom;

enum class TokenType : char  { Null, Atom, Symbol, Id };
typedef struct Token {
  TokenType type;
  union Val {
    Atom atom;
    Symbol sym;
    std::string id;
    Val() {}
    ~Val() {}
  } val;

  Token(const Token& t) : type(t.type) {
    if (t.type == TokenType::Atom)
      new (&val.atom) Atom(t.val.atom);
    else if (t.type == TokenType::Symbol)
      val.sym = t.val.sym;
    else if (t.type == TokenType::Id)
      new (&val.id) std::string(t.val.id);
    else type = TokenType::Null;
  }

  ~Token() {
    if (type == TokenType::Id)
      val.id.std::string::~string();
    if (type == TokenType::Atom)
      val.atom.~Atom();
  }

  Token() : type(TokenType::Null) {}
  Token(const Atom& v) : type(TokenType::Atom) { new (&val.atom) Atom(v); }
  Token(const Symbol& v) : type(TokenType::Symbol) { val.sym = v; }
  Token(const std::string& v) : type(TokenType::Id) { new (&val.id) std::string(v); }

  Token(Token&& other) noexcept : type(other.type) {
    if (other.type == TokenType::Atom)
      new (&val.atom) Atom(std::move(other.val.atom));
    else if (other.type == TokenType::Symbol)
      val.sym = other.val.sym;
    else if (other.type == TokenType::Id)
      new (&val.id) std::string(std::move(other.val.id));
    other.type = TokenType::Null;
  }

  Token& operator=(Token&& other) noexcept {
    if (this == &other) return *this;
    this->~Token();
    type = other.type;
    if (other.type == TokenType::Atom)
      new (&val.atom) Atom(std::move(other.val.atom));
    else if (other.type == TokenType::Symbol)
      val.sym = other.val.sym;
    else if (other.type == TokenType::Id)
      new (&val.id) std::string(std::move(other.val.id));
    other.type = TokenType::Null;
    return *this;
  }
} Token;

namespace std {
  template<> void swap(Token& a, Token& b) noexcept {
    Token temp(std::move(a));
    a = std::move(b);
    b = std::move(temp);
  }
}

typedef struct Op {
  OpType type;
  union Val {
    Atom atom;
    int num;
    Val() {}
    ~Val() {}
  } val;
  Op(const Op& c) : type(c.type) {
    if (type == OpType::Atom)
      new (&val.atom) Atom(c.val.atom);
    else if (type >= OpType::Var && type <= OpType::Lor)
      val.num = c.val.num;
  }
  Op& operator=(const Op& c) {
    if (this == &c) return *this;

    if (type == OpType::Atom)
      val.atom.~Atom();

    type = c.type;
    if (type == OpType::Atom)
      new (&val.atom) Atom(c.val.atom);
    else if (type >= OpType::Var && type <= OpType::Lor)
      val.num = c.val.num;
    return *this;
  }
  ~Op() {
    if (type == OpType::Atom)
      val.atom.~Atom();
  }
  Op(OpType type) : type(type) {}
  Op(const Atom& v) : type(OpType::Atom) { new (&val.atom) Atom(v); }
  Op(OpType type, int v) : type(type) { val.num = v; }
} Op;

#endif
