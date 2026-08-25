\# 2D SAT Collision Tag Game (OpenGL / GLUT)


## Overview

This project is an interactive 2D Tag Game implemented in **C** using **OpenGL** and the **GLUT** utility toolkit. The engine demonstrates real-time 2D geometry transformations, smooth vector interpolation, and precise collision detection between arbitrary 2D convex polygons using the **Separating Axis Theorem (SAT)**.

In this game, the player controls a customizable Square using keyboard commands while fleeing from or tagging a Mouse-controlled Triangle that smoothly interpolates toward target coordinates.


\## Controls

\- \*\*Square Movement:\*\* `W`, `A`, `S`, `D`

\- \*\*Square Rotation:\*\* `F` (Counter-Clockwise), `G` (Clockwise)

\- \*\*Triangle Movement:\*\* Mouse Left Click / Drag

\- \*\*Speed Adjustments:\*\* `1`/`2` (Square), `3`/`4` (Triangle)

\- \*\*Restart:\*\* `R`


## Key Features

* **Separating Axis Theorem (SAT) Collision Engine:** Custom-built SAT algorithm calculating normal edge vectors and 1D interval projections for accurate collision detection between rotated polygons.
* **Dynamic Geometry Transformations:** Local-to-world coordinate transformation using trigonometric rotation matrices ($x' = x \cos\theta - y \sin\theta$).
* **Smooth Target Interpolation:** Constant-velocity dynamic tracking system for smooth movement toward mouse coordinates.
* **Real-time Speed Adjustments:** Live keyboard bindings to adjust movement speeds for both shapes independently during execution.
* **On-Screen UI & HUD:** Custom bitmap text rendering displaying active entity velocities and game-over state overlays.


## Technical Specifications & Architecture

| Component | Implementation Details |
| :--- | :--- |
| **Language & Graphics** | C language, OpenGL 1.x pipeline, freeGLUT/GLUT framework |
| **Collision Algorithm** | Separating Axis Theorem (SAT) on 2D convex hulls |
| **Math Operations** | `math.h` single-precision float operations, 2D vector primitives (`Vec2`) |
| **Input Handling** | GLUT Keyboard (`glutKeyboardFunc`) and Mouse Motion callbacks |
| **Frame Rate Control** | `glutTimerFunc` tuned for ~60 FPS update loops |



