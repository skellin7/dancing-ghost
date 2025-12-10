# Project 7

Sarah Liao, Kelly Lin



#### Movement

* Click \& drag near the limb you wish to control to move the left/right wrists and ankles.
* Hold the left/right arrow keys to play the left/right movement animations.
* Use the W/A/S/D/ctrl/space keys to pan the camera.
* Settings on the side allow limbs/head/torso to be resized.



#### Cloth

* Settings on side allow rendering as vertices \& springs, normal-colored fabric, and texture-colored fabric.
* Settings on side also control various other cloth properties.





#### Known Bugs

* Individual limb control through mouse movement works unless camera/figure location is too far from origin, where mouse tracking calculations mean that target location is far off from actual desired location.
* Initial cloth generation may appear to not properly collide with figure; adjusting the cloth to shape collision slider will fix issue.
* If cloth behaves oddly, regenerate cloth or tune various parameters.
