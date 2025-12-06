## Project 6: Final Project Gear Up

The project handout can be found [here](https://cs1230.graphics/projects/final/gear-up).

### Test Cases



Insert your test cases here. Be sure to include images/videos as needed!

Kinematic Skeletons + Linear Blend Skinning
- Renders vampire (hard-coded input file) defined by a mesh and shaded with textures
- Creates kinematic skeleton of vampire based on defined bone structure
- Vampire's mesh undergoes transformations based on bones assigned to each vertex within mesh
- Vampire moves (i.e. mesh is rendered in different configurations) as per animation defined by keyframes of bone transforms

https://github.com/user-attachments/assets/e4252a81-b7bc-425c-b63b-0789c8fbe4ea

Kinematic Skeletons + Inverse Kinematics
- Renders line art stick figure drawing representing skeleton of a person
- Creates kinematic skeleton of person based on simple bone structure
- When mouse is dragged, right wrist bone attempts to follow mouse position
- Rotation of shoulder and elbow are calculated via inverse kinematics to enable wrist to follow mouse

https://github.com/user-attachments/assets/7126f496-b09f-408e-bd53-3ab795a48705


### Design Choices

Macro in realtime.cpp determines whether linear blend skinning version or inverse kinematics version are rendered.
- Implementation of kinematic skeletons differs between two versions because requirements for data stored is much simpler for inverse kinematic demonstration.

### Collaboration/References

### Known Bugs

### Extra Credit
