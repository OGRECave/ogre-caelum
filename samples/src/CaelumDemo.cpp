/*
This file is part of Caelum.
See http://www.ogre3d.org/wiki/index.php/Caelum 

Copyright (c) 2006-2007 Caelum team. See Contributors.txt for details.

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

#include "CaelumDemo.h"

using Caelum::LongReal;

void testAlmostEqual (LongReal x, LongReal y, LongReal d) {
    assert (d > 0);
    LongReal dif = x - y;
    if (dif < 0) {
        dif = -dif;
    }

    std::stringstream stream;
    stream.precision (15);
    stream.setf(std::ios::fixed);
    stream << "Difference is " << dif << ", allowed is " << d;

    // Ogre log not created yet. Don't rely on it.
    //Ogre::LogManager::getSingleton ().logMessage (stream.str ());
    std::cout << stream.str() << std::endl;
    assert (dif < d);
}

void checkJulianDay (double jd,
        int year, int month, int day,
        int hour, int minute, Caelum::LongReal second)
{
    int cyear, cmonth, cday, chour, cminute;
    LongReal calcJd;
    int calcIjd;
    LongReal csecond;
    
    // Round-trip integer julian day.
    calcIjd = Caelum::Astronomy::getJulianDayFromGregorianDate (year, month, day);
    Caelum::Astronomy::getGregorianDateFromJulianDay (calcIjd, cyear, cmonth, cday);
    assert (cyear == year);
    assert (cmonth == month);
    assert (cday == day);

    // Check floating-point julian day.
    calcJd = Caelum::Astronomy::getJulianDayFromGregorianDateTime
            (year, month, day, hour, minute, second);
    assert (Ogre::Math::RealEqual (jd, calcJd));

    Caelum::Astronomy::getGregorianDateTimeFromJulianDay (calcJd,
            cyear, cmonth, cday, chour, cminute, csecond);
    assert (cyear == year);
    assert (cmonth == month);
    assert (cday == day);
    assert (chour == hour);
    assert (cminute == minute);
    // We can lose a lot of precision.
    testAlmostEqual (csecond, second, 0.1);
}

void checkSunPosition (
        LongReal jday,
        LongReal longitude, LongReal latitude,
        LongReal azimuth, LongReal altitude)
{
    LongReal cazimuth, caltitude;
    Caelum::Astronomy::getHorizontalSunPosition (
            jday, longitude, latitude, cazimuth, caltitude);
    // Allow for large errors.
    testAlmostEqual (cazimuth, azimuth, 5);
    testAlmostEqual (caltitude, altitude, 5);
}   

void checkSunPosition (
        int year, int month, int day,
        int hour, int minute, LongReal second,
        LongReal longitude, LongReal latitude,
        LongReal azimuth, LongReal altitude)
{
    LongReal jday = Astronomy::getJulianDayFromGregorianDateTime
            (year, month, day, hour, minute, second);
    checkSunPosition (jday, longitude, latitude, azimuth, altitude);
}

void checkMoonPosition (
        LongReal jday,
        LongReal longitude, LongReal latitude,
        LongReal azimuth, LongReal altitude)
{
    LongReal cazimuth, caltitude;
    Caelum::Astronomy::getHorizontalMoonPosition (
            jday, longitude, latitude, cazimuth, caltitude);
    // Allow for large errors.
    testAlmostEqual (cazimuth, azimuth, 5);
    testAlmostEqual (caltitude, altitude, 5);
}   

void checkMoonPosition (
        int year, int month, int day,
        int hour, int minute, LongReal second,
        LongReal longitude, LongReal latitude,
        LongReal azimuth, LongReal altitude)
{
    LongReal jday = Astronomy::getJulianDayFromGregorianDateTime
            (year, month, day, hour, minute, second);
    checkMoonPosition (jday, longitude, latitude, azimuth, altitude);
}

void checkAstronomy () {
    std::cout << "Testing julian day" << std::endl;
    // Tested with http://aa.usno.navy.mil/data/docs/JulianDate.php
    checkJulianDay (2451545.0, 2000, 1, 1, 12, 0, 0);
    checkJulianDay (2451544.5, 2000, 1, 1, 0, 0, 0);
    checkJulianDay (2451544.49994, 1999, 12, 31, 23, 59, 55);
    checkJulianDay (2451543.5, 1999, 12, 31, 0, 0, 0);
    checkJulianDay (2452986.01094, 2003, 12, 12, 12, 15, 45);
    //The Julian date for CE 1976 February 24 01:10:55.0 UT is JD 2442832.54925
    checkJulianDay (2442832.54925, 1976,  2, 24,  1, 10, 55);
    //The Julian date for CE 1845 November 11 07:03:20.0 UT is JD 2395246.79398
    checkJulianDay (2395246.79398, 1845, 11, 11,  7,  3, 20);
    //The Julian date for CE  2067 April  1 23:59:59.0 UT is JD 2476107.49999
    checkJulianDay (2476107.49999, 2067,  4,  1, 23, 29, 59);

    std::cout << "Testing sun position" << std::endl;
    // Various javascript calculators on the web.
    checkSunPosition (2448000.5, 15, 60, 15.6767, -17.9570);
    checkSunPosition (2442832.54925, -15, 3, 185.1, -83.1);
    checkSunPosition (2008, 1, 4, 14, 46, 17,
            -82.63, 27.97, 136.8552, 24.936968432703356);
    checkSunPosition (2000, 12, 3, 12, 34, 0,
            -81.70, 41.00, 118.93, -0.84);

    // Eclipse test, http://www.satellite-calculations.com/Satellite/suncalc.htm
    checkSunPosition (1999, 8, 11, 11, 3, 0,
            24.3, 45.1, 197.1877, 60.2837);
    checkSunPosition (2005, 2, 1,  3, 4, 5,
            1.68, 48.08, 61.4531, -44.5082);

    std::cout << "Testing moon position" << std::endl;
    checkMoonPosition (1999, 8, 11, 11, 3, 0,
            24.3, 45.1, 197.2246, 60.2921);
    checkMoonPosition (1995, 6, 3, 18, 10, 0,
            1.68, 48.08, 225.1021, 45.8011);
    checkMoonPosition (2005, 2, 1,  3, 4, 5,
            1.68, 48.08, 147.0071, 24.4169);
}

extern "C" int main (int argc, char **argv)
{
    // Run astronomy math tests.
    // Allow for very large precission errors. Javascript web calculators
    // are probably even less precise.
    // The tests are only used to catch huge visible errors.
    checkAstronomy ();	
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
