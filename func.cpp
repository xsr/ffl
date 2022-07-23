#include "func.h"
#include "parser/srcread.h"
#include "parser/lexer.h"
#include "parser/symtable.h"
#include <getopt.h>
#include <fstream>
#include "git.h"

#include "lib.hpp"

namespace funC {
int verbosity = -1, indent = 1, opt_level = 2;
bool stack_layout_comments = true, op_rewrite_comments = true, program_envelope = true, asm_preamble = false;

std::ostream* outs = &std::cout;
std::string generated_from, boc_output_filename;

void generate_output_func(SymDef* func_sym) {
  SymValCodeFunc* func_val = dynamic_cast<SymValCodeFunc*>(func_sym->value);
  assert(func_val);
  std::string name = sym::symbols.get_name(func_sym->sym_idx);
  if (verbosity >= 2) {
    std::cerr << "\n\n=========================\nfunction " << name << " : " << func_val->get_type() << std::endl;
  }
  if (!func_val->code) {
    std::cerr << "( function `" << name << "` undefined )\n";
  } else {
    CodeBlob& code = *(func_val->code);
    if (verbosity >= 3) {
      code.print(std::cerr, 9);
    }
    code.simplify_var_types();
    if (verbosity >= 5) {
      std::cerr << "after simplify_var_types: \n";
      code.print(std::cerr, 0);
    }
    code.prune_unreachable_code();
    if (verbosity >= 5) {
      std::cerr << "after prune_unreachable: \n";
      code.print(std::cerr, 0);
    }
    code.split_vars(true);
    if (verbosity >= 5) {
      std::cerr << "after split_vars: \n";
      code.print(std::cerr, 0);
    }
    for (int i = 0; i < 8; i++) {
      code.compute_used_code_vars();
      if (verbosity >= 4) {
        std::cerr << "after compute_used_vars: \n";
        code.print(std::cerr, 6);
      }
      code.fwd_analyze();
      if (verbosity >= 5) {
        std::cerr << "after fwd_analyze: \n";
        code.print(std::cerr, 6);
      }
      code.prune_unreachable_code();
      if (verbosity >= 5) {
        std::cerr << "after prune_unreachable: \n";
        code.print(std::cerr, 6);
      }
    }
    code.mark_noreturn();
    if (verbosity >= 3) {
      code.print(std::cerr, 15);
    }
    if (verbosity >= 2) {
      std::cerr << "\n---------- resulting code for " << name << " -------------\n";
    }
    bool inline_ref = (func_val->flags & 2);
    *outs << std::string(indent * 2, ' ') << name << " PROC" << (inline_ref ? "REF" : "") << ":<{\n";
    code.generate_code(
        *outs,
        (stack_layout_comments ? Stack::_StkCmt | Stack::_CptStkCmt : 0) | (opt_level < 2 ? Stack::_DisableOpt : 0),
        indent + 1);
    *outs << std::string(indent * 2, ' ') << "}>\n";
    if (verbosity >= 2) {
      std::cerr << "--------------\n";
    }
  }
}

int generate_output() {
  if (asm_preamble) {
    *outs << "\"Asm.fif\" include\n";
  }
  *outs << "// automatically generated from " << generated_from << std::endl;
  if (program_envelope) {
    *outs << "PROGRAM{\n";
  }
  for (SymDef* func_sym : glob_func) {
    SymValCodeFunc* func_val = dynamic_cast<SymValCodeFunc*>(func_sym->value);
    assert(func_val);
    std::string name = sym::symbols.get_name(func_sym->sym_idx);
    *outs << std::string(indent * 2, ' ');
    if (func_val->method_id.is_null()) {
      *outs << "DECLPROC " << name << "\n";
    } else {
      *outs << func_val->method_id << " DECLMETHOD " << name << "\n";
    }
  }
  for (SymDef* gvar_sym : glob_vars) {
    assert(dynamic_cast<SymValGlobVar*>(gvar_sym->value));
    std::string name = sym::symbols.get_name(gvar_sym->sym_idx);
    *outs << std::string(indent * 2, ' ') << "DECLGLOBVAR " << name << "\n";
  }
  int errors = 0;
  for (SymDef* func_sym : glob_func) {
    try {
      generate_output_func(func_sym);
    } catch (src::Error& err) {
      std::cerr << "cannot generate code for function `" << sym::symbols.get_name(func_sym->sym_idx) << "`:\n"
                << err << std::endl;
      ++errors;
    }
  }
  if (program_envelope) {
    *outs << "}END>c\n";
  }
  if (!boc_output_filename.empty()) {
    *outs << "2 boc+>B \"" << boc_output_filename << "\" B>file\n";
  }
  return errors;
}

}  // namespace funC

std::string generate(const std::string & _src) {
  std::istringstream srcs(_src);
  std::ostringstream dsts;

  funC::define_keywords();
  funC::define_builtins();

  funC::parse_source_stream(&srcs);

  std::ostream* tmp = funC::outs;  
  funC::outs = &dsts;
  funC::generate_output();
  funC::outs = tmp;
  return dsts.str();
}
