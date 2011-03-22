/*
This file is part of Caelum.
See http://www.ogre3d.org/wiki/index.php/Caelum 

Copyright (c) 2008 Caelum team. See Contributors.txt for details.

Caelum is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Caelum is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Caelum. If not, see <http://www.gnu.org/licenses/>.
*/

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
