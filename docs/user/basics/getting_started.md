# Getting started

## System Requirements

QtRvSim is built using the Qt framework and supports multiple platforms. Please ensure your system meets the following minimum requirements before installation.

### Supported Operating Systems
**Windows**
- Windows 7 or later

**macOS**
- macOS 10.10 (Yosemite) or later

**Linux**
- Modern 64-bit distributions
- Dedicated packages available for Ubuntu 18.04+, openSUSE, Fedora, Debian, Raspbian/Raspberry Pi OS, CentOS 8, Arch Linux, and Flatpak-supported distributions
- NixOS/Nix support included

**WebAssembly (Browser-based)**
- Latest versions of Chrome, Firefox, Edge, or Safari
- Available at [comparch.edu.cvut.cz/qtrvsim/app](https://comparch.edu.cvut.cz/qtrvsim/app)

## Installation

Download the appropriate installation files from the official QtRvSim releases page: <https://github.com/cvut/qtrvsim/releases>

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

### Linux Installation

Choose the installation method that best suits your distribution:

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

#### Flatpak (Universal Package)
```bash
flatpak install flathub cz.cvut.edu.comparch.qtrvsim
```
Launch with:
```bash
flatpak run cz.cvut.edu.comparch.qtrvsim
```

#### NixOS/Nix

Install the package directly from the official `nixpkgs` repository.

**Permanent installation**

Add `qtrvsim` to your `configuration.nix` file:

```nix
environment.systemPackages = [
    pkgs.qtrvsim
];
```

**Temporary usage**

```bash
nix-shell -p qtrvsim --run qtrvsim_gui
```