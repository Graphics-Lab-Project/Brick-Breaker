#pragma once
// OWNER: Phase 0 (interface FROZEN). Implementation: task "Ball physics" -> BallPhysics.cpp
#include "Types.h"
#include <QVector>

namespace BB::BallPhysics {

// Paddle reflection. t = clamp((ballX - paddleX) / (paddleWidth / 2), -1, 1) (t = 0 if width <= 0).
// angle = t * MaxBounceAngleDeg from vertical. Returns (speed*sin(angle), -speed*cos(angle)).
Vec2 paddleBounce(qreal ballX, qreal paddleX, qreal paddleWidth, qreal speed);

// Launch from the paddle: LaunchAngleDeg from vertical, upward, toward dir
// (dir < 0 -> left, otherwise right). Magnitude == speed.
Vec2 launchVelocity(int dir, qreal speed);

// Four upward velocities at MultiAnglesDeg (-50, -20, 20, 50 from vertical), in that order.
QVector<Vec2> multiBallVelocities(qreal speed);

// Left / right / top walls of the field. On contact the ball centre is moved back inside
// (centre x in [3, FieldW-3], centre y >= 3) and the matching velocity component is made
// to point away from that wall. Returns true if any wall was hit. The bottom is open.
bool bounceWalls(Ball &b);

// True once the ball's top edge (pos.y - BallHalf) is below FieldH.
bool isLost(const Ball &b);

// Ball resting on the paddle: pos (paddleX, BallRestY), vel (0, 0).
Ball restingBall(qreal paddleX);

// Same direction, new magnitude. A zero vector becomes (0, -speed).
Vec2 withSpeed(const Vec2 &v, qreal speed);

} // namespace BB::BallPhysics
