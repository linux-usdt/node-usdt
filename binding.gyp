{
  "targets": [
    {
      "target_name": "usdt",
      "sources": [
        "usdt-provider.cc",
        "usdt-probe.cc",
      ],
      "include_dirs": ["<!(node -e \"require('nan')\")"],
      "libraries": [
        "-lstapsdt"
      ]
    }
  ]
}
