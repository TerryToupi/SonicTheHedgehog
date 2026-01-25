#pragma once

#include "Scene/Sprite.h"
#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"

scene::MotionQuantizer::Mover 
MakeSpriteGridLayerMoverWithCamera(scene::GridMap* map, scene::Sprite* sprite, scene::TileLayer* tiles);

scene::MotionQuantizer::Mover 
MakeSpriteGridLayerMover(scene::GridMap* map, scene::Sprite* sprite);
