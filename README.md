```
    _______      .---.         ____     .-------.
   /   __  \     | ,_|       .'  __ `.  \  _(`)_ \
  | ,_/  \__)  ,-./  )      /   '  \  \ | (_ o._)|
,-./  )        \  '_ '`)    |___|  /  | |  (_,_) /
\  '_ '`)       > (_)  )       _.-`   | |   '-.-'
 > (_)  )  __  (  .  .-'    .'   _    | |   |
(  .  .-'_/  )  `-'`-'|___  |  _( )_  | |   |
 `-'`-'     /    |        \ \ (_ o _) / /   )
   `._____.'     `--------`  '.(_,_).'  `---'

```

**CL**ever **A**udio **P**lugin.

# Learn about CLAP

The starting point is [clap.h](include/clap/clap.h).

The two most important objects are `clap_host` and `clap_plugin`.

Most features comes from extensions, which are in fact C interfaces.
```C
// host extension
const clap_host_log *log = host->extension(host, CLAP_EXT_LOG);
if (log)
   log->log(host, CLAP_LOG_INFO, "Hello World! ;^)");

// plugin extension
const clap_plugin_params *params = plugin->extension(plugin, CLAP_EXT_PARAMS);
if (params)
{
   uint32_t paramsCount = params->count(plugin);
   // ...
}
```

The extensions are defined in [ext](include/clap/ext) folder.

Some extensions are still in the progress of being designed and they are in
the [draft](include/clap/ext/draft) folder.

An extension comes with:
- an header `#include <clap/ext/xxx.h>`
- an extension identifier: `#define CLAP_EXT_XXX "clap/XXX"`
- host interfaces are named like: `struct clap_host_xxx`
- plugin interfaces are named like: `struct clap_plugin_xxx`
- each methods must have a clear thread specification

You can create your own extensions and share them, make sure that the extension identifier
- includes versioning in case the ABI breaks
- a unique identifier

**All strings are valid UTF-8**.

## Fundamental extensions

This is a list of the extensions that you most likely want to implement
and use to get a basic plugin experience:
- [log](include/clap/ext/log.h), lets the host aggregate plugin logs
- [thread-check](include/clap/ext/thread-check.h), check which thread you are currently on, useful for correctness validation
- [audio-ports](include/clap/ext/audio-ports.h), define and configure the ports layout
- [params](include/clap/ext/params.h), parameters management
- [latency](include/clap/ext/latency.h), report the plugin latency
- [state](include/clap/ext/state.h), save and load the plugin state

## GUI extensions

- [gui](include/clap/ext/gui.h), generic gui controller
- [gui-win32](include/clap/ext/gui-win32.h), win32 specific
- [gui-cocoa](include/clap/ext/gui-cocoa.h), cocoa specific
- [gui-x11](include/clap/ext/gui-x11.h), x11 specific
- [timer-support](include/clap/ext/timer-support.h), lets the plugin register timer handlers
- [fd-support](include/clap/ext/fd-support.h), lets the plugin register I/O handlers

## Extra extensions

- [render](include/clap/ext/render.h), renders realtime or offline
- [note-name](include/clap/ext/draft/note-name.h), give a name to notes, useful for drum machines
- [tuning](include/clap/ext/draft/tuning.h), host provided microtuning
- [track-info](include/clap/ext/draft/track-info.h)
- [thread-pool](include/clap/ext/draft/thread-pool.h), use the host thread pool
- [quick-controls](include/clap/ext/draft/quick-controls.h), bank of controls that can be mapped on a controlles with 8 knobs
- [file-reference](include/clap/ext/draft/file-reference.h), let the host know about the plugin's file reference, and perform "Collect & Save"
- [check-for-update](include/clap/ext/draft/check-for-update.h), check if there is a new version of a plugin

## Examples

Visit the [examples](examples) folder.

## Building on various platforms

### macOS

To build the example host on macOS you need a few extra libraries, qt6, boost, portmidi and portaudio.
These are all available by homebrew and the CMake setup will find them assuming a standard
(/usr/local) homebrew setup. Before your first build do

```shell
brew install qt6
brew install boost
brew install portaudio
brew install portmidi
brew install pkgconfig
```

### Windows

For the example host you'll need to install Qt6, PortAudio and PortMidi.  

Qt6 requires you to create an account during installation, but you can also compile it from source. If you're building with MSVC, you cannot use the prebuilt Qt6 (mingw) or you'll get linker errors. For building from source:
´´´powershell
choco install python strawberryperl ninja -y # install Python3, Ninja and Perl
git clone -b 6.2.1 git://code.qt.io/qt/qt5.git
cd qt5
```

Now, if you want to initialize only the submodules (dependencies) necessary for clap:
```powershell
perl init-repository --module-subset=essential,addon,-qtwebengine,-qtactiveqt,-qt3d,-qtlottie,-qtdatavis3d,-qtgamepad,-qtpurchasing,-qtquick3d,-qtquickcontrols,-qtquicktimeline,-qtspeech,-qtvirtualkeyboard,-qtwayland,-qtsensors,-qtserialbus,-qtserialport,-qtscxml,-qtremoteobjects,-qtqa
```

For initializing with default settings (a lot more dependencies):
```powershell
perl init-repository
```

Open a `x64 Native Tools Command Prompt for VS 2019` terminal, go to the newly cloned Qt git repo and run:
```batch
mkdir /c/Qt/6.2.1
mkdir build
cd build
..\configure.bat -release -no-pch -prefix "c:/Qt/6.2.1-git" -skip qtwebengine -nomake tools -nomake tests -nomake examples
::The following command failed on some of my machines, but repeating it multiple times made it work at some point ...
cmake --build . --parallel
cmake --install .
```

After building/installation, you'll have to put the `bin` path to the %PATH% system environment variable, like `C:\Qt\6.2.1\mingw81_64\bin` (downloaded installer) or `C:\Qt\6.2.1` (built from source). Additionally, add the `QT_PLUGIN_PATH` environment variable with `C:\Qt\6.2.1\mingw81_64\plugins` (downloaded installer) or `C:\Qt\6.2.1-git\plugins` (built from source).

The rest of the dependencies will be handled through vcpkg. So checkout the vcpkg submodule included in this repository:
```powershell
git submodule init
git submodule update
```

From now on, CMake should be able to automatically detect vcpkg, bootstrap and acquire the necessary packages.

The generated solutions will have two problems that need manual fixing atm. You'll have to fix the "Additional Dependencies" path in the clap-host solution in the linker settings from `portmidi.lib` to `..\..\vcpkg_installed\x64-windows\debug\lib\portaudio.lib`. Also, the "Additional options" compiler settings should not have the `no-vadli-offset` parameter removed (happens only sometimes).
