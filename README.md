# Space-Poitiers-Shooter

openFrameworks project. Draw a space shooter video game through etherdream/ilda laser beamer.



(+ 3 modified addons)

ofxIldaKush : + allows to draw objects with different quality/point spacing. Allows trapeze distortion, rotation.

ofxGamepadKush : + sends controller ID in the callback function.

ofxEtherDreamKush : + allows to connect via IP address string.



Draw as many players as there are xBox controllers plugged.

The players who survive more than 30 seconds is the winner.

Invulnerability & LSD bonuses spawn every 30s.



https://www.youtube.com/watch?v=ZZ4NZ7JylgI



**Player controls**

left stick X axis : rotate

A button : Fire

right trigger : accelerate

left trigger : brake



**Level editor**

"j" : switch play/edit modes (0:play 1: places boxes 2: places spawn points)

A button : places new object

B button : removes selected object

left stick : moves selected object

right stick : scales selected object

right trigger : rotate selected object

right/left bumper : select another object



**Laser configuration**

zqsd : scale

arrows : position

e/E : rotate

o/O/p/P : trapeze

see more in testApp.cpp



k : save level & config to file (data.dat, will then load the new config when the program starts again)
