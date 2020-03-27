{
  "conditions": [
    [
      'OS=="win"',
      {

         "targets":[
        {
          "target_name": "build_safex",
          "type": "none",
          "actions": [
            {
              "action_name": "retrieve_from_github",
              "inputs": "",
              "outputs": [
                "../deps/libwin_wallet_wrapper.dll",
              ],
              "action": [
                "make deps","--directory=.."],
              "message": "Building windows safex libraries"
            }
          ]
        },
        {
          "target_name": "safex",
          "dependencies": [
            "build_safex"
          ],
          "sources": [
            "src/addon.cc",
            "src/wallet.cc",
            "src/walletcallbacks.cc",
            "src/walletargs.cc",
            "src/deferredtask.cc",
            "src/wallettasks.cc",
            "src/pendingtransaction.cc",
            "src/win/winwallet.cpp",
            "src/win/winwalletlistener.cpp",
            "src/win/winwalletmanager.cpp",
            "src/win/winpendingtransaction.cpp",
            "src/win/misc.cpp"
          ],
          "libraries": [
            '-l../deps/libwin_wallet_wrapper.lib' 
          ],
          "include_dirs": [
            "include",
            "src",
            "src/win",
            "<!(node -e \"require('nan')\")"
          ]
        },
        {
          "target_name": "action_after_build",
          "type": "none",
          "dependencies": [
            "<(module_name)"
          ],
          "copies": [
            {
              "files": [
                "<(PRODUCT_DIR)/<(module_name).node",
                "./deps/libwin_wallet_wrapper.dll",
                "/msys64/mingw64/bin/libwinpthread-1.dll",
                "/msys64/mingw64/bin/libgcc_s_seh-1.dll",
                "/msys64/mingw64/bin/libstdc++-6.dll"
              ],
              "destination": "<(module_path)"
            }
          ]
        }
      ]
      },
      { #nonwin
      "conditions":[
        [
          'OS=="linux"',
          {
            "variables": {
              'additional_libraries': [
                "-lpcsclite"
              ]
            }
          }
        ],
        [
          'OS=="mac"',
          {
            "variables": {
              'additional_libraries': [
                "-framework PCSC"
              ]
            }
          }
        ]
      ],
      "targets":[
        {
          "target_name": "build_safex",
          "type": "none",
          "actions": [
            {
              "action_name": "retrieve_from_github",
              "inputs": "",
              "outputs": [
                "../deps/libwallet_merged.a",
                "../deps/libepee.a",
                "../deps/libeasylogging.a",
                "../deps/liblmdb.a",
                "../deps/libunbound.a"
              ],
              "action": [
                "make",
                "deps"
              ],
              "message": "Building safex libraries"
            }
          ]
        },
        {
          "target_name": "safex",
          "dependencies": [
            "build_safex"
          ],
          "sources": [
            "src/addon.cc",
            "src/wallet.cc",
            "src/walletcallbacks.cc",
            "src/walletargs.cc",
            "src/deferredtask.cc",
            "src/wallettasks.cc",
            "src/pendingtransaction.cc"
          ],
          "libraries": [
            "../deps/libwallet_merged.a",
            "../deps/libepee.a",
            "../deps/libeasylogging.a",
            "../deps/liblmdb.a",
            "../deps/libunbound.a",
            "../deps/libboost_serialization.a",
            "../deps/libboost_thread.a",
            "../deps/libboost_system.a",
            "../deps/libboost_date_time.a",
            "../deps/libboost_filesystem.a",
            "../deps/libboost_chrono.a",
            "../deps/libboost_program_options.a",
            "../deps/libboost_regex.a",
            "<@(additional_libraries)",
            "-lssl",
            "-lcrypto",
            "-lz",
          ],
          "include_dirs": [
            "include",
            "<!(node -e \"require('nan')\")"
          ]
        },
        {
          "target_name": "action_after_build",
          "type": "none",
          "dependencies": [
            "<(module_name)"
          ],
          "copies": [
            {
              "files": [
                "<(PRODUCT_DIR)/<(module_name).node"
              ],
              "destination": "<(module_path)"
            }
          ]
        }
      ]
      }
    ]
  ]
}
