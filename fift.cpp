#include "td/utils/port/path.h"
#include "crypto/fift/words.h"
#include "crypto/fift/Fift.h"

#include "lib.hpp"
#include "fifsrc.hpp"

std::string interpret(const std::string & src) {
  std::ostringstream output;

  // std::string current_dir = td::realpath(".").move_as_ok();
  std::string current_dir = "";
  // std::string lib_dir = td::realpath("./lib/").move_as_ok();
  fift::SourceLookup source_lookup = fift::SourceLookup(std::make_unique<fift::OsFileLoader>());
  // source_lookup.add_include_path(lib_dir);
  fift::Dictionary dictionary;
  fift::init_words_common(dictionary);
  fift::init_words_vm(dictionary, true);  // enable vm debug
  fift::init_words_ton(dictionary);
  fift::IntCtx ctx;

  ctx.source_lookup = &source_lookup;
  ctx.dictionary = &dictionary;
  ctx.output_stream = &output;
  ctx.error_stream = &std::cerr;

  // auto fift_lib_stream = std::make_unique<std::stringstream>(std::move(source_lookup.lookup_source("Fift.fif", lib_dir).ok().data));

  // auto fift_lib_stream = std::make_unique<std::stringstream>(std::move(fift_fif_src));
  // ctx.enter_ctx("Fift.fif", current_dir, std::move(fift_lib_stream));
  // ctx.run(td::make_ref<fift::InterpretCont>());

  // auto source_stream = std::make_unique<std::stringstream>(std::move(src));
  // ctx.enter_ctx("stdin", current_dir, std::move(source_stream));
  // ctx.run(td::make_ref<fift::InterpretCont>());

  return output.str();
}
