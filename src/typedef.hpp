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
  Null, Str, I64, F64,
  Var, Asg, AsgPop, Call, List, Jmp, Jift, Jiff, Land, Lor
};

enum class AtomType : char  { None, Null, I64, F64, Str, List, StdFn };
typedef struct Atom Atom;
typedef void (*ZnStdFn)(size_t, Atom*);

struct Atom {
  AtomType type;
  union Val {
    long long i64;
    double f64;
    std::string str;
    std::vector<Atom> list;
    ZnStdFn stdfn;
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
    else if (a.type == AtomType::StdFn)
      val.stdfn = a.val.stdfn;
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
    else if (a.type == AtomType::StdFn)
      val.stdfn = a.val.stdfn;
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
    else if (type == AtomType::StdFn)
      val.stdfn = a.val.stdfn;
    else type = AtomType::Null;

    return *this;
  }

  Atom& operator=(Atom&& a) {
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
      new (&val.str) std::string(std::move(a.val.str));
    else if (type == AtomType::List)
      new (&val.list) std::vector<Atom>(std::move(a.val.list));
    else if (type == AtomType::StdFn)
      val.stdfn = a.val.stdfn;
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
  Atom(long long v) : type(AtomType::I64) { val.i64 = v; }
  Atom(double v) : type(AtomType::F64) { val.f64 = v; }
  Atom(const std::string& v) : type(AtomType::Str) { new (&val.str) std::string(v); }
  Atom(const std::vector<Atom>& v) : type(AtomType::List) { new (&val.list) std::vector<Atom>(v); }
  Atom(ZnStdFn v) : type(AtomType::StdFn) { val.stdfn = v; }
};

enum class TokenType : char  { Null, Atom, Symbol, Id };
struct Token {
  enum TokenType type;
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
};

namespace std {
  template<> void swap(Token& a, Token& b) noexcept {
    Token temp(std::move(a));
    a = std::move(b);
    b = std::move(temp);
  }
}

struct Op {
  OpType type;
  union Val {
    std::string str;
    long long i64;
    double f64;
    size_t usize;
    ssize_t size;
    Val() {}
    ~Val() {}
  } val;
  Op(const Op& o) : type(o.type) {
    if (type == OpType::Str)
      new (&val.str) std::string(o.val.str);
    else if (type == OpType::I64)
      val.i64 = o.val.i64;
    else if (type == OpType::F64)
      val.f64 = o.val.f64;
    else if (type >= OpType::Var && type <= OpType::List)
      val.usize = o.val.usize;
    else if (type >= OpType::Jmp && type <= OpType::Lor)
      val.size = o.val.size;
  }
  Op& operator=(const Op& o) {
    if (this == &o) return *this;

    if (type == OpType::Str)
      val.str.std::string::~string();

    type = o.type;
    if (type == OpType::Str)
      new (&val.str) std::string(o.val.str);
    else if (type == OpType::I64)
      val.i64 = o.val.i64;
    else if (type == OpType::F64)
      val.f64 = o.val.f64;
    else if (type >= OpType::Var && type <= OpType::List)
      val.usize = o.val.usize;
    else if (type >= OpType::Jmp && type <= OpType::Lor)
      val.size = o.val.size;
    return *this;
  }
  ~Op() {
    if (type == OpType::Str)
      val.str.std::string::~string();
  }
  Op(OpType type) : type(type) {}
  Op(std::string str) : type(OpType::Str) {new (&val.str) std::string(str);}
  Op(long long i64) : type(OpType::I64) { val.i64 = i64; }
  Op(double f64) : type(OpType::F64) { val.f64 = f64; }
  Op(OpType type, size_t usize) : type(type) { val.usize = usize; }
  Op(OpType type, ssize_t size) : type(type) { val.size = size; }
};

#endif
