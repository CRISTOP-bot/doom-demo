{
  "targets": [
    {
      "target_name": "doom_native",
      "sources": [
        "native/doom_binding.cc",
        "native/game_logic.c"
      ],
      "include_dirs": [
        "native",
        "<!(node -p \"require('node-addon-api').include_dir\")"
      ],
      "cflags": [
        "-std=c99",
        "-Wall",
        "-O3"
      ],
      "cflags_cc": [
        "-std=c++17",
        "-Wall",
        "-O3"
      ],
      "conditions": [
        [
          "OS=='linux'",
          {
            "cflags": ["-fPIC"],
            "cflags_cc": ["-fPIC"]
          }
        ]
      ]
    }
  ]
}
