{
  inputs = {
    nixpkgs.url      = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url  = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
      in with pkgs;
      {
        devShells.default = mkShell {
          buildInputs = [
            cmake
            glm
            glfw
            shaderc
            vulkan-headers
            vulkan-loader
            vulkan-validation-layers
            freetype
          ];
          VULKAN_SDK = "${vulkan-headers}";
          VK_LAYER_PATH = "${vulkan-validation-layers}/share/vulkan/explicit_layer.d";
          LD_LIBRARY_PATH="${glfw}/lib:${freetype}/lib:${vulkan-loader}/lib:${vulkan-validation-layers}/lib";

        };
      }
    );
}

