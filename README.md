Description
===========

This is a small programm to display some information you need to be notified
about. It also monitors you volume and shows it. You can use DBus or a TCP
connection to show text or a progress bar.

Requirements
============

You need the following packages to be installed.
- Qt >=4.4 (http://www.trolltech.com)
- ALSA (http://www.alsa-project.org/)

Install
=======
Just do a "qmake && make" and copy the binary "qtosd" somewhere to your path.

Configuration & usage
=====================

There are some parameters you can use:

    -b, --background FILE      Set FILE as background image
    -p, --port PORT            Use PORT as TCP port for incoming connections
    -t, --timeout TIME         Show OSD for TIME seconds
    -d, --device DEVICE        Use this device instead of default
    -n, --no-daemon            Don't start in daemon mode
    -w, --width                Width of OSD
    -h, --height               Height of OSD
    -s, --screen SCREEN        Show OSD on SCREEN
    -sb, --space-bottom HEIGHT Free space below OSD
    -sl, --space-left HEIGHT   Free space left of OSD
    -f, --fade                 Fade in/out OSD
    -m, --move                 Move in/out OSD

The text you send can also have a special format:

    some text
      -> shows just "some text"
    
    1/some text
      -> shows only one line of text (usefull for toggling options)
    
    10/100%some text
      -> shows a progress bar and use 10 as value, 100 as max and "some text".
    
    f:/path/to/file.jpg%text
      -> displays file.jpg on the left side and text
    
    f:/path/to/file.jpg%1/10%text
      -> displays image, progressbar and text

Have also a look at the examples.

Background
==========
For the background you have to use a SVG file. Just create it with a size ratio
of 800:150 (pixels). For other values you have to set width and height.  To use
translucency you need a compositing window manager or you have to use xcompmgr.

Have fun ;)
