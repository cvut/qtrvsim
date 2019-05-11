# App icon on macOS

Qt Docs: [Setting the Application Icon on macOS](https://doc.qt.io/qt-5/appicon.html#setting-the-application-icon-on-macos)

**qtmips_qui.svg** icon is exported using [Affinity Designer](https://affinity.serif.com/en-gb/designer/)
in **all required dimensions together with the manifest for the Apple iconutil**. Export config and the icon are saved in `qtmips_qui.afdesign`.

## Steps to generate macOS icon

Requirements: macOS with XCode (iconutil)

1. Open **qtmips_qui.afdesign** in [Affinity Designer](https://affinity.serif.com/en-gb/designer/).
2. Switch to the **Export Persona** and run **Export Slices**.
	You will be asked to select export folder.
	In that folder, **icon.iconset** folder will be created.
	**icon.iconset** contains rendered icons for all required dimensions together with the **Contents.json manifest** file.
3. Generate **qtmips_qui.icns** using Apple's `iconutil` _(supply the correct path to the previously created icon.iconset):_
	```bash
	iconutil iconutil --convert icns --output qtmips_qui.icns icon.iconset
	```
	**Few caveats**:
	- Iconset folder must have **.iconset** extension. Otherwise icontuil fails with error: _Invalid Iconset._
	- Names of icons in Iconset must start with icon_ (case sensitive). Otherwise icontuil fails with error: _Failed to generate ICNS._
4. Copy generated qtmips_qui.icns to project's `data/icons/macos/qtmips_qui.icns`.
5. Finally copy `data/icons/macos/qtmips_qui.icns` to `qtmips_qui/icons/qtmips_qui.icns`
	
**Note:** [Affinity Designer](https://affinity.serif.com/en-gb/designer/) is not required. It is possible to create .iconset folder with the icons manually.
However, it is good idea to automatize the .iconset creation (i.e. use Affinity Designer or other tools).



