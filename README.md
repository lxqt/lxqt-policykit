# lxqt-policykit

## Overview

lxqt-policykit is the polkit authentification agent of LXQt.

[polkit](https://www.freedesktop.org/wiki/Software/polkit/) is a software framework to handle privileges of processes.
In LXQt it is e. g. used to extend the privileges of the GUI tools of [lxqt-admin](https://github.com/lxde/lxqt-admin/).
These are launched by a regular user. But in order to apply the settings that can be adjusted root privileges are needed
and acquired via polkit.   

Among the various components of polkit the authentication agent is the one to query the user for credentials and hence
comes with its own GUI like this one of lxqt-policykit.

![lxqt-policykit: GUI](lxqt-policykit_gui.png)

While there's only a single implementation of all other polkit components there are various different authentication
agents provided by the various desktop environments. Normally these can be used interchangeably, that is lxqt-policykit
can be used in an LXDE session or lxpolkit, the authentication agent of LXDE, can be used in an Xfce session. Normally
it's better to use the implementation provided by a distinct desktop environment as it integrates better, though.

Technically, lxqt-policykit is just a single binary `lxqt-policykit-agent` which has to be running all over the LXQt
sessions and is therefor launched as so-called LXQt Module, see [Configuration](#configuration).

Note that the naming lxqt-policykit is strictly speaking an anachronism. It refers to Policykit which was the predecessor
of polkit. The name wasn't changed when Policykit was replaced by polkit as both provide roughly the same features albeit
they are technically different.

## Installing

### Compiling sources

Runtime dependencies are [liblxqt](https://github.com/lxde/liblxqt) and its dependency
[libqtxdg](https://github.com/lxde/libqtxdg) as well as polkit-qt5, to build Git and CMake are needed in addition.   
By default lxqt-policykit is pulling translations from repository [translations](https://github.com/lxde/translations/) at
compile time, see file README.md of the latter.

To compile, run `cmake`, `make` and `make install`.
`cmake` can be invoked in an out of source build directory and will normally need variable `-DCMAKE_INSTALL_PREFIX=/usr`.
`make install` can be invoked with `DESTDIR=<some path>`.

### Binary packages

#### Arch Linux

The latest release is provided by package `lxqt-policykit` in repository community, the actual master checkout can be
compiled by packages `lxqt-policykit-git` from the [AUR](https://aur.archlinux.org).

#### Debian

Package `lxqt-policykit` is available in the official repositories as of Debian stretch. For now it is comprising the
translations as well but these will probably be outsourced in package `lxqt-policykit-l10n` one day.

#### Fedora

Package `lxqt-policykit` is available as of Fedora 22.

#### openSUSE

Package `lxqt-policykit` is providing the binary, `lxqt-policykit-lang` the translations. Both are available as of
openSUSE Leap 42.1.

## Configuration

As mentioned in section [Overview](#overview) binary `lxqt-policykit-agent` is started as LXQt Module, which is a particular
way of LXQt to launch binaries at the beginning of an LXQt session and keep them running all the time.   
This is handled by desktop entry file `/etc/xdg/autostart/lxqt-policykit-agent.desktop`. It belongs to
[lxqt-common](https://github.com/lxde/lxqt-common/) and is featuring key `OnlyShowIn=LXQt;` restricting it to LXQt sessions.
So no further action has to be taken as far as LXQt is concerned. In order to use lxqt-policykit in other sessions the key has
to be modified accordingly and/or removed, optionally after copying the file to a user specific directory, normally
`~/.config/autostart/`.

## Usage

From a user's point of view the usage is limited to interacting with the GUI as seen above.
