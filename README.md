scene
=====

The goal of this game is to reach the end of the
map (i.e. the finish line) in as little as time
possible.  Movement is accomplished with w, a, s,
and d.  you can choose maps using the numbers.
Map number 0 is a blank slate that you can
customize.  if you press the ` key (to the left
of the one) you can enter the editing mode.  where
you click it will add a tree.  if you press the
spacebar instead of adding trees on clicks it will
raise the terrain, spacebar again and it will lower
the terrain. c will clear everything, this mode is
very experimental. esc key exits the program.  When 
you reach the finish line it will give you a time, 
and a score, based on how much air you caught.

all textures are cited in the code and taken from
people who are allowing users to use them for non
commercial purposes.  Many of them were edited
slightly by myself in order to look better or be
small enough for me to reasonably load.  all of the
maps except for level 3 were made by me.  it is
fairly easy to make them.  just make a 128x128 .tga
file in an image editor (such as gimp) and when you
export make sure to turn off RLE. your character
will travel from left to right on you map twice.
Lighter regions indicate a higher place on the map
and you must make sure that both ends of the map
(front and back) are of constant height, or the
terrain wont correctly overlap.

most of my basic goals were implemented, with the
exception of the guy who walks around, I decided
to make this a game, and so I have a snowcat that
you drive around.  I would also like to have more
naturey things other than the rocks.  I intend on
adding three more things.  First a snow trail. I
want the cat to leave a snow trail as it goes, I
am thinking I will change the texture at the
location of the cat as it moves along.  I also
want smoke coming out of the little chimney on
the snow cat.  I also want to add snow, using a
particle engine.  As I move along I am sure I will
come up with new things to work on as well.

If you would like to view my current progress this
project can be seen at:
https://github.com/coghex/scene

to compile, go into the directory and enter:
>make

this will output scene, then:
>./scene
