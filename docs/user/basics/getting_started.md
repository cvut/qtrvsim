# Getting started

## System Requirements

QtRvSim is built using the Qt framework and supports multiple platforms. Please ensure your system meets the following minimum requirements before installation.

### Supported Operating Systems

- **Linux** (64-bit distributions with Qt 5.12+)
- **macOS** (10.10 (Yosemite) or later)
- **Windows** (Windows 7 or later)
- **WebAssembly (Browser-based)**
  - Modern versions of Firefox, Chromium-based browsers or Safari

## Installation

Download the appropriate installation files from the official QtRvSim releases page: <https://github.com/cvut/qtrvsim/releases>

### WebAssembly

Just open [comparch.edu.cvut.cz/qtrvsim/app](https://comparch.edu.cvut.cz/qtrvsim/app) in your web browser to run QtRvSim without any installation.


### Linux Installation

Choose the installation method that best suits your distribution:

[![Packaging status](https://repology.org/badge/vertical-allrepos/qtrvsim.svg)](https://repology.org/project/qtrvsim/versions)

#### Ubuntu 18.04+ and Derivatives

```bash
sudo add-apt-repository ppa:qtrvsimteam/ppa
sudo apt-get update
sudo apt-get install qtrvsim
```

#### Arch, CentOS, Debian, Fedora, openSUSE, Raspbian/Raspberry Pi OS

1. Visit the [Open Build Service download page](https://software.opensuse.org/download.html?project=home%3Ajdupak&package=qtrvsim)
2. Select your specific distribution and version
3. Follow the provided instructions to add the repository and install it

#### Arch Linux (AUR)

You can install QtRvSim from the Arch User Repository (AUR) using an AUR helper like `yay`:

```bash
yay -S qtrvsim
# or
yay -S qtrvsim-git  # for the latest development version
```


#### Flatpak (Universal Package)
```bash
flatpak install flathub cz.cvut.edu.comparch.qtrvsim
```
Launch with:
```bash
flatpak run cz.cvut.edu.comparch.qtrvsim
```

#### NixOS/Nix

Install the package directly from the official `nixpkgs` [repository](https://search.nixos.org/packages?channel=unstable&show=qtrvsim&query=qtrvsim).

<details>
<summary><strong>Permanent installation</strong></summary>

Add `qtrvsim` to your `configuration.nix` file:

```nix
environment.systemPackages = [
    pkgs.qtrvsim
];
```

</details>

<details>
<summary><strong>Temporary usage</strong></summary>

```bash
nix-shell -p qtrvsim --run qtrvsim_gui
```

</details>

### Windows Installation

1. Download the Windows archive: `qtrvsim-mingw32-[version].zip`
2. Extract the contents to your preferred location (e.g., `C:\QtRvSim`)
3. Launch `qtrvsim.exe` from the extracted folder
4. No additional installation steps required

### macOS Installation

#### Method 1: Disk Image (Recommended)
1. Download the macOS disk image: `qtrvsim-macos-[version].dmg`
2. Open the .dmg file
3. Drag the QtRvSim application to your Applications folder
4. **First launch**: Right-click the application in Applications, select "Open," and confirm if prompted by Gatekeeper

#### Method 2: Archive File
1. Download the macOS archive: `qtrvsim-macos-[version].zip`
2. Extract the QtRvSim.app bundle
3. Move QtRvSim.app to your Applications folder
4. Follow step 4 from Method 1 for first launch
