{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = {nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
  in {
    packages.${system}.default = pkgs.callPackage ({
      lib,
      pkg-config,
      stdenv,
      meson,
      ninja,
      scdoc,
      wayland,
      wayland-scanner,
      wayland-protocols,
      cairo,
      libxkbcommon,
      buildDocs ? true,
    }:
      stdenv.mkDerivation {
        name = "slurp";
        src = ./.;

        depsBuildBuild = [pkg-config];

        nativeBuildInputs =
          [
            meson
            ninja
            pkg-config
            wayland-scanner
          ]
          ++ lib.optional buildDocs scdoc;

        buildInputs = [
          cairo
          libxkbcommon
          wayland
          wayland-protocols
        ];

        strictDeps = true;

        mesonFlags = [(lib.mesonEnable "man-pages" buildDocs)];
      }) {};

    devShells.${system}.default = pkgs.mkShell {
      packages = with pkgs; [
        meson
        wayland-protocols
        wayland-scanner
        wayland
        ninja
        cairo
        libxkbcommon
        scdoc
        llvmPackages.clang-tools
        pkg-config
        cmake
      ];
    };
  };
}
