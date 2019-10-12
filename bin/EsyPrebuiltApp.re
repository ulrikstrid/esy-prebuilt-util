let target = ref("");
let package = ref("");
let from = ref("");
let to_ = ref("");
let action = ref("");

/**
EsyPrebuilt.exe -action export -target $(pwd)/'#{. / os}' -package '#{fswatch.install}'
EsyPrebuilt.exe -action import -from $(pwd)/'#{. / os}'
*/
Arg.parse_argv(
  Sys.argv,
  [
    (
      "-action",
      Arg.Symbol(["export", "import"], a => action := a),
      "Export from store or import into store",
    ),
    (
      "-target",
      Arg.Set_string(target),
      "Should probably be #{self.root / os}",
    ),
    ("-from", Arg.Set_string(from), "Should probably be #{self.root / os}"),
    (
      "-to",
      Arg.Set_string(to_),
      "Should probably be #{self.install}, if the length doesn't match you're going to have a bad time",
    ),
    ("-package", Arg.Set_string(package), "Package name to be exported"),
  ],
  extras => {print_endline("Not used: " ++ extras)},
  "",
);

print_endline(action^);
print_endline(target^);
print_endline(package^);

switch (action^) {
| "export" =>
  let packageDir = package^;
  let storePrefix = String.init(String.length(packageDir), _ => '_');
  let targetDir = target^;

  Library.copy_and_replace(
    ~packageDir,
    ~replace=packageDir,
    ~storePrefix,
    ~targetDir,
  );
| "import" =>
  let targetDir = to_^;
  let storePrefix = String.init(String.length(targetDir), _ => '_');

  Library.copy_and_replace(
    ~packageDir=from^,
    ~replace=storePrefix,
    ~storePrefix=targetDir,
    ~targetDir,
  );
| _ => print_endline("No valid action")
};
