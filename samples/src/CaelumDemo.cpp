// This file is part of the Caelum project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution.

#include "CaelumDemo.h"

int main (int argc, char **argv)
{
    try {
        chdirExePath (argv[0]);

        CaelumSampleApplication app;
        app.go ();
        return 0;

    } catch (Exception& e) {
        reportException (e.getFullDescription ().c_str ());
    } catch (std::exception& e) {
        reportException (e.what ());
    }

    return 1;
}
