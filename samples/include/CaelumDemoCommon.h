// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "Caelum.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <unistd.h>
#endif

using namespace Caelum;

/** Report an exception as a message box or a stderr message.
 */
inline void reportException (const char* msg) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    MessageBoxA(NULL, msg, "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
    std::cerr << "An exception has occured: " << msg << std::endl;
#endif
}

/** Change diretory to the exe's path; based on argv0
 */
inline void chdirExePath (char* argv0) {
    std::string exePath (argv0);
    std::string::size_type last_pos = exePath.find_last_of ("\\/");
    if (last_pos != std::string::npos) {
        std::string exeDir = exePath.substr (0, last_pos);
        std::cerr << "Chdir to " << exeDir << std::endl;
        chdir (exeDir.c_str ());
    }
}
