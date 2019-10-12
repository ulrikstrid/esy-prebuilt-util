# esy-prebuilt

Helper util to create prebuilts for esy

## Usage

The command has 2 modes, export and import.

### When creating the prebuilts - exporting

```
EsyPrebuiltApp.exe -action export -target '#{self.root / os}' -package '#{fswatch.install}'
```

### When installing the prebuilts - importing

```
EsyPrebuiltApp.exe -action import -from '#{self.root / os}' -to '#{self.install}'
```
