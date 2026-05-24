#ifndef ZN_FUNC_HPP
#define ZN_FUNC_HPP

#include "includes.hpp"

void f_exit(int argc, Atom* atoms) {
  if (argc == 0) exit(0);
  if (atoms->type != AtomType::I64) throw "TypeError: except int";
  exit(atoms->val.i64);
}

void f_bool(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::I64;
  atoms[-1].val.i64 = atob(*atoms);
}
void f_int(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::I64;
  atoms[-1].val.i64 = atoi(*atoms);
}
void f_str(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::Str;
  atoms[-1].val.str = atos(*atoms);
}
void f_repl(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::Str;
  atoms[-1].val.str = arepl(*atoms);
}

void f_out(int argc, Atom* atoms) {
  for (int i = 0; i < argc; i++)
    std::cout << atos(atoms[i]);
  atoms[-1].type = AtomType::Null;
}
void f_outln(int argc, Atom* atoms) {
  for (int i = 0; i < argc; i++)
    std::cout << atos(atoms[i]);
  std::cout << std::endl;
  atoms[-1].type = AtomType::Null;
}
void f_input(int argc, Atom* atoms) {
  for (int i = 0; i < argc; i++)
    std::cout << atos(atoms[i]);
  std::string str;
  std::getline(std::cin, str);
  atoms[-1].type = AtomType::Str;
  atoms[-1].val.str = str;
}

void f_clock(int argc, Atom* atoms) {
  if (argc != 0) throw "ArgError: expect 0 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = (double)clock() / CLOCKS_PER_SEC;
}
void f_sleep(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  if (atoms->type != AtomType::F64) throw "TypeError: except float";
  std::this_thread::sleep_for(std::chrono::microseconds((int64_t)(atoms->val.f64 * 1000000)));
}

void f_sin(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = sin(atoms->val.f64);
}
void f_cos(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = cos(atoms->val.f64);
}
void f_tan(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = tan(atoms->val.f64);
}
void f_asin(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = asin(atoms->val.f64);
}
void f_acos(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = acos(atoms->val.f64);
}
void f_atan(int argc, Atom* atoms) {
  if (argc != 1) throw "ArgError: expect 1 argument";
  atoms[-1].type = AtomType::F64;
  atoms[-1].val.f64 = atan(atoms->val.f64);
}

#endif
