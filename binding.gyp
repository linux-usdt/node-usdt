{
  "targets": [
    {
      "target_name": "usdt",
      "sources": [
        "usdt-provider.cc",
        "usdt-probe.cc",
      ],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "libraries": [
        "-lstapsdt"
      ]
    }
  ]
}
