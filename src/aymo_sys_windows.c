/*
AYMO - Accelerated YaMaha Operator
Copyright (c) 2023-2024 Andrea Zoppi.

This file is part of AYMO.

AYMO is free software: you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 2.1 of the License, or (at your option)
any later version.

AYMO is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with AYMO. If not, see <https://www.gnu.org/licenses/>.
*/

#include "aymo_cc.h"
#if (defined(AYMO_CC_HOST_WINDOWS) || defined(AYMO_CC_HOST_CYGWIN))

#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

AYMO_CXX_EXTERN_C_BEGIN


BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH: {
            // Initialize once for each new process.
            // Return FALSE to fail DLL load.

            // Thread optimization.
            DisableThreadLibraryCalls(hinstDLL);
            break;
        }
        case DLL_THREAD_ATTACH: {
            // Do thread-specific initialization.
            break;
        }
        case DLL_THREAD_DETACH: {
            // Do thread-specific cleanup.
            break;
        }
        case DLL_PROCESS_DETACH: {
            if (lpvReserved) {
                // Do not do cleanup if process termination scenario.
                break;
            }
            // Perform any necessary cleanup.
            break;
        }
        default: break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}


AYMO_CXX_EXTERN_C_END

#endif  // (defined(AYMO_CC_HOST_WINDOWS) || defined(AYMO_CC_HOST_CYGWIN))
