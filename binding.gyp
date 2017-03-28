{
  "targets": [
    {
      "target_name": "png",
      "sources": [
        "src/png.cpp",
        "src/module.cpp",
      ],
      "include_dirs": [
        "include",
        "<!(node -e \"require('nan')\")"
      ],
      "conditions" : [
        [
          'OS=="linux"', {
            "libraries" : [
              '-lpng',
              '-lz'
            ],
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ]
          }
        ],
        [
          'OS=="mac"', {
            'xcode_settings': {
              'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
            },
            "libraries" : [
              '<!@(pkg-config libpng --libs)'
            ]
          }
        ],
        [
          'OS=="win"', {
            "include_dirs" : [ "gyp/include" ],
            "libraries" : [
              '<(module_root_dir)/gyp/lib/libpng.lib',
              '<(module_root_dir)/gyp/lib/zlib.lib'
            ]
          }
        ]
      ]
    }
  ]
}
