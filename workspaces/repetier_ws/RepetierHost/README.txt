Installation instructions

1. Open a terminal
2. Go to the directory, where you want to have the installation
3. tar -xzf repetierHostLinux.tgz
   change tar file accordingly.
4. cd RepetierHost
5. sh configureFirst.sh
6. Make sure your user has permission to use the serial port. On
   Debian and Fedora this requires membership in group dialout. The
   script will try to add you to dialout automatcally.
   To add a user into this group enter:
      usermod -a -G dialout yourUserName
7. This directory will now contain a Repetier-Host.desktop file, that you can copy to
   your desktop to have a icon to start the host with.
      
After that, you have a link in /usr/bin so you can start the host from
everywhere with

repetierHost

Known issues:
- You may see an OpenGL warning at startup. Ignore it.
- Sometimes the start fails. Just start again.
- Some mono versions seem to crash on exit. Ignore the messages as you are already leaving.
- You need a recent mono version like 3.2 or the host will crash sooner or later due to
  not implemented functions.



