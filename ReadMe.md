
# Introduction {#mainpage}

%Caelum is a plug-in/library for %Ogre targeted to help create nice-looking (photorealistic if possible) atmospheric effects such as sky colour, clouds and weather phenomena such as rain or snow. It is composed of a number of small mostly self-contained components and a big Caelum::CaelumSystem class which ties them all together in an easy-to-use way.

For more information see the big %Caelum thread on the ogre forum:  
    http://forums.ogre3d.org/viewtopic.php?t=24961  
The thread is very long; you should try reading it backwards.

There is also a page on the ogre wiki (might be out of date):  
    http://wiki.ogre3d.org/Caelum

# Features

* Sky colour; depending on time of the day.
* Drawing the sun and moon (including phases).
* Multiple animated cloud layers with controllable intensity.
* Astronomically correct sun and moon position based on calendar date and geographic position.
* Point-based starfield based on bright star catalogue.
* Precipitation compositor. It's a non-particle implementation of precipitation which is very fast on larger displays.
* Depth-based fog compositor. It can provide more complex fogging without requiring changes in your own materials.
* Easy to integrate into your own project.

# Usage

See CaelumDemo's source code; it should contain some minimal initialisation and tweaking. CaelumLab is more complicated and can be used to visually tweak different sky elements.

There is no %Caelum "SDK"; the recommended way to use it is to build it yourself.
