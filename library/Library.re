let directoryUnlikelyToAffectBuild = path =>
  switch (Fp.baseName(path)) {
  | None => false
  | Some(bn) =>
    String.equal(bn, "npm-packages-offline-cache")
    || String.equal(bn, "_build")
    || String.equal(bn, "esy.lock")
    || String.equal(bn, ".ci")
    || String.equal(bn, ".hg")
    || String.equal(bn, ".git")
    || String.equal(bn, "node_modules")
    || String.equal(bn, "_esy")
  };

let onDir = (~path_cb, ~file_cb, queryResult, cont) => {
  switch (queryResult) {
  | Fs.Link(path, _, _) => ()
  | Other(path, _, _) => ()
  | File(path, _) => file_cb(path)
  | Dir(path, _) =>
    !directoryUnlikelyToAffectBuild(path)
      ? {
        path_cb(path);
        cont();
      }
      : ()
  };
};

let path_cb = (~abs_root, ~abs_target, dest) => {
  Fp.relativize(~source=abs_root, ~dest)
  |> Base.Result.map(~f=Fp.join(abs_target))
  |> Base.Result.map(~f=Fs.mkDir)
  |> ignore;
};

let file_cb = (~abs_root, ~abs_target, ~pattern, ~with_, dest) => {
  open Base;

  let replacePattern = String.Search_pattern.create(pattern);
  Fp.relativize(~source=abs_root, ~dest)
  |> Result.map(~f=relativePath => {
       Fs.readBinary(dest)
       |> Result.map(~f=str => {
            String.Search_pattern.replace_all(
              replacePattern,
              ~in_=str,
              ~with_,
            )
          })
       |> Result.map(~f=Fs.writeBinary(Fp.join(abs_target, relativePath)))
     })
  |> ignore;
};

let copy_and_replace = (~packageDir, ~replace, ~storePrefix, ~targetDir) => {
  let abs_root = Fp.absoluteExn(packageDir);
  let abs_target = Fp.absoluteExn(targetDir);

  Fs.mkDir(abs_target) |> ignore;

  Fs.traverseFileSystemFromPath(
    ~onNode=
      onDir(
        ~file_cb=
          file_cb(
            ~abs_root,
            ~abs_target,
            ~pattern=replace,
            ~with_=storePrefix,
          ),
        ~path_cb=path_cb(~abs_root, ~abs_target),
      ),
    abs_root,
  );
};
