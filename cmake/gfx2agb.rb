MRuby::Build.new do |conf|
  # load specific toolchain settings
  conf.toolchain

  # No executables
  conf.bins = []

  # Do not build executable test
  conf.build_mrbtest_lib_only

  # Disable C++ exception
  conf.disable_cxx_exception

  # Gems from core
  # removing mruby-io
  conf.gem core: "mruby-metaprog"
  conf.gem core: "mruby-pack"
  conf.gem core: "mruby-sprintf"
  conf.gem core: "mruby-print"
  conf.gem core: "mruby-math"
  conf.gem core: "mruby-time"
  conf.gem core: "mruby-struct"
  conf.gem core: "mruby-compar-ext"
  conf.gem core: "mruby-enum-ext"
  conf.gem core: "mruby-string-ext"
  conf.gem core: "mruby-numeric-ext"
  conf.gem core: "mruby-array-ext"
  conf.gem core: "mruby-hash-ext"
  conf.gem core: "mruby-range-ext"
  conf.gem core: "mruby-proc-ext"
  conf.gem core: "mruby-symbol-ext"
  conf.gem core: "mruby-random"
  conf.gem core: "mruby-object-ext"
  conf.gem core: "mruby-objectspace"
  conf.gem core: "mruby-fiber"
  conf.gem core: "mruby-enumerator"
  conf.gem core: "mruby-enum-lazy"
  conf.gem core: "mruby-toplevel-ext"
  conf.gem core: "mruby-kernel-ext"
  conf.gem core: "mruby-class-ext"
  conf.gem core: "mruby-compiler"
end
