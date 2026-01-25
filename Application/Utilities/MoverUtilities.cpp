#include "MoverUtilities.h"

scene::MotionQuantizer::Mover 
MakeSpriteGridLayerMoverWithCamera(scene::GridMap* map, scene::Sprite* sprite, scene::TileLayer* tiles)
{
	return [map, sprite, tiles] (Rect& r, int* dx, int* dy) {
		int windowX = tiles->GetViewWindow().x;	
		int windowY = tiles->GetViewWindow().y;
		int spriteX = sprite->GetBox().x;

		map->FilterGridMotion(r, dx, dy);
		if (*dx || *dy)
		{
			sprite->SetHasDirectMotion(true);
			sprite->Move(*dx, *dy); 
			sprite->SetHasDirectMotion(false);

			if (tiles->CanScrollHoriz(*dx))
			{
				tiles->Scroll(spriteX - windowX - windowY / 2, 0);
			}
		}
	};
}

scene::MotionQuantizer::Mover 
MakeSpriteGridLayerMover(scene::GridMap* map, scene::Sprite* sprite)
{
	return [map, sprite] (Rect& r, int* dx, int* dy) {
		if(*dx || *dy)
		{
			sprite->SetHasDirectMotion(true);
			sprite->Move(*dx, *dy);
			sprite->SetHasDirectMotion(false);
		}
	};
}
