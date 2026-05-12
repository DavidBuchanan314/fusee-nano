{
  description = "fusee-nano - A minimalist re-implementation of the Fusée Gelée exploit";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages = {
          fusee-nano = pkgs.stdenv.mkDerivation {
            pname = "fusee-nano";
            version = "0.5.3";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              xxd
              gcc
              gnumake
            ];

            buildPhase = ''
              make
            '';

            installPhase = ''
              make install PREFIX=$out/bin
            '';

            meta = with pkgs.lib; {
              description = "A minimalist re-implementation of the Fusée Gelée exploit, designed to run on embedded Linux devices. (Zero dependencies)";
              license = licenses.mit;
              platforms = platforms.linux;
            };
          };

          default = self.packages.${system}.fusee-nano;
        };

        apps = {
          fusee-nano = {
            type = "app";
            program = "${self.packages.${system}.fusee-nano}/bin/fusee-nano";
          };

          default = self.apps.${system}.fusee-nano;
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            xxd
            gcc
            gnumake
          ];
        };
      }
    );
}
