# README for vexterm

## Building
There currently is nothing to configure. 
Just type 'make dep' and 'make'.

## Dependencies
* glib
* gtk
* pango
* cairo
* popt
* expat

on a Debian-based system one could type:

    apt-get install libglib2.0-dev libgtk2.0-dev libpango1.0-dev libcairo2-dev libpopt-dev libexpat1-dev

## Configuration
you can configure Vexterm using the 'Options' -> 'Preferences' menu.
To have some profiles and colourschemes predefined, do this:

    cp res/config/config_global.xml /usr/share/vexterm/config.xml

and

    cp res/config/config_local.xml $HOME/.config/vexterm/config.xml

The global config file will be for all users and not editable by
them. But they can use these profiles to duplicate them to create 
their own and locally overwrite preferences configured in the 
global file.
