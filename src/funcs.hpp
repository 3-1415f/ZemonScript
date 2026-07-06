#ifndef ZN_FUNC_HPP
#define ZN_FUNC_HPP

#include "includes.hpp"

void f_exit(size_t argc, Atom* atoms) {
  if (argc == 0) exit(0);
  if (atoms->type != AtomType::I64) throw "TypeError: except int";
  exit(atoms->val.i64);
}

void f_bool(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::I64;
  atoms[-1].val.i64 = atob(*atoms);
}
void f_int(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::I64;
  atoms[-1].val.i64 = atoi(*atoms);
}
void f_str(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::Str;
  atoms[-1].val.str = atos(*atoms);
}

void f_type(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::Str;
  switch (atoms->type) {
    case AtomType::Null: atoms[-1].val.str = "null"; break;
    case AtomType::I64: atoms[-1].val.str = "int"; break;
    case AtomType::F64: atoms[-1].val.str = "float"; break;
    case AtomType::Str: atoms[-1].val.str = "str"; break;
    case AtomType::List: atoms[-1].val.str = "list"; break;
    case AtomType::StdFn: atoms[-1].val.str = "function"; break;
  }
}

void f_repl(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::Str;
  atoms[-1].val.str = arepl(*atoms);
}

void f_out(size_t argc, Atom* atoms) {
  for (int i = 0; i < argc; i++)
    std::cout << atos(atoms[i]);
  atoms[-1].type = AtomType::Null;
}
void f_outln(size_t argc, Atom* atoms) {
  for (int i = 0; i < argc; i++)
    std::cout << atos(atoms[i]);
  std::cout << std::endl;
  atoms[-1].type = AtomType::Null;
}
void f_input(size_t argc, Atom* atoms) {
  for (int i = 0; i < argc; i++)
    std::cout << atos(atoms[i]);
  std::string str;
  std::getline(std::cin, str);
  atoms[-1].type = AtomType::Str;
  atoms[-1].val.str = str;
}

void f_clock(size_t argc, Atom* atoms) {
  if (argc != 0) throw "ArgError: expect 0 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = (double)clock() / CLOCKS_PER_SEC;
}
void f_sleep(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type != AtomType::F64) throw "TypeError: except float";
  std::this_thread::sleep_for(std::chrono::microseconds((int64_t)(atoms->val.f64 * 1000000)));
}

void f_sin(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type == AtomType::F64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = sin(atoms->val.f64);
  } else if (atoms->type == AtomType::I64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = sin((double)atoms->val.i64);
  } else throw "TypeError: except float or int";
}
void f_cos(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type == AtomType::F64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = cos(atoms->val.f64);
  } else if (atoms->type == AtomType::I64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = cos((double)atoms->val.i64);
  } else throw "TypeError: except float or int";
}
void f_tan(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type == AtomType::F64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = tan(atoms->val.f64);
  } else if (atoms->type == AtomType::I64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = tan((double)atoms->val.i64);
  } else throw "TypeError: except float or int";
}
void f_asin(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type == AtomType::F64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = asin(atoms->val.f64);
  } else if (atoms->type == AtomType::I64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = asin((double)atoms->val.i64);
  } else throw "TypeError: except float or int";
}
void f_acos(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type == AtomType::F64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = acos(atoms->val.f64);
  } else if (atoms->type == AtomType::I64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = acos((double)atoms->val.i64);
  } else throw "TypeError: except float or int";
}
void f_atan(size_t argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type == AtomType::F64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = atan(atoms->val.f64);
  } else if (atoms->type == AtomType::I64) {
    atoms[-1].type = AtomType::F64;
    atoms[-1].val.f64 = atan((double)atoms->val.i64);
  } else throw "TypeError: except float or int";
}

#endif
