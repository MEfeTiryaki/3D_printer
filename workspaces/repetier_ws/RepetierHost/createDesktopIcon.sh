#!/bin/bash
sudo cp RepetierHostMimeTypes.xml /usr/share/mime/packages/
sudo update-mime-database /usr/share/mime
user=$(whoami)
dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
mono="$(which mono)"
echo "[Desktop Entry]
Name=Repetier-Host
Exec=$mono $dir/RepetierHost.exe -home $dir
Type=Application
StartupNotify=true
Comment=Repetier-Host 3d printer host software
Path=$dir
Icon=$dir/repetier-logo.png
MimeType=application/sla;text/gcode;application/wavefront-obj;application/x-amf;
" > $dir/repetier-RepetierHost.desktop
echo "<!DOCTYPE Menu PUBLIC "-//freedesktop//DTD Menu 1.0//EN"
          "http://www.freedesktop.org/standards/menu-spec/menu-1.0.dtd">
          <Menu>
            <Name>Applications</Name>
                <Filename>shinythings-webmirror.desktop</Filename>
                <Filename>shinythings-webmirror-admin.desktop</Filename>
            </Menu>
" > $dir/Repetier-Host.menu
chmod +x $dir/Repetier-Host.desktop
sudo desktop-file-install --rebuild-mime-info-cache repetier-RepetierHost.desktop
echo "This folder now contains a repetier-RepetierHost.desktop file"
echo "Copy it to your desktop to get a launch icon there"
