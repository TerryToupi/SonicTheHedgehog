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
MakeSpriteGridLayerMover(scene::GridMap* map, scene::Sprite* sprite, int gridYOffset)
{
	return [map, sprite, gridYOffset] (Rect& r, int* dx, int* dy) {
		// Adjust rect for grid Y offset (grid doesn't start at world y=0)
		Rect gridRect = { r.x, r.y - gridYOffset, r.w, r.h };

		// Save original dy to detect jumping (before collision filtering)
		int originalDy = *dy;

		// Do collision check if ANY part of sprite could overlap with grid
		// gridRect.y + gridRect.h > 0 means bottom edge is at or below grid top
		if (gridRect.y + gridRect.h > 0)
		{
			map->FilterGridMotion(gridRect, dx, dy);
		}

		if (*dx || *dy)
		{
			sprite->SetHasDirectMotion(true);
			sprite->Move(*dx, *dy);
			sprite->SetHasDirectMotion(false);

			// Update r for next quantizer step - critical for multi-step collision
			r.x += *dx;
			r.y += *dy;

			// Ground-following after horizontal movement
			// Only when walking on ground (not jumping - originalDy >= 0)
			if (*dx != 0 && originalDy >= 0)
			{
				Rect newGridRect = { r.x, r.y - gridYOffset, r.w, r.h };

				// Check for step-up (walking into higher ground)
				int snapUp = map->GetGroundSnapDistance(newGridRect);
				if (snapUp > 0)
				{
					// Before snapping up, verify we won't end up inside a horizontal wall
					// This prevents snapping onto walls when ground meets a wall
					Rect snappedRect = { newGridRect.x, newGridRect.y - snapUp, newGridRect.w, newGridRect.h };

					// Probe if we can move even 1 pixel horizontally from the snapped position
					int probeLeft = -1, probeRight = 1;
					int zeroDy = 0;
					Rect testRectLeft = snappedRect;
					Rect testRectRight = snappedRect;
					map->FilterGridMotion(testRectLeft, &probeLeft, &zeroDy);
					zeroDy = 0;
					map->FilterGridMotion(testRectRight, &probeRight, &zeroDy);

					// Only snap up if we can still move horizontally (not stuck inside a wall)
					bool insideWall = (probeLeft == 0 && probeRight == 0);
					if (!insideWall)
					{
						sprite->SetHasDirectMotion(true);
						sprite->Move(0, -snapUp);
						sprite->SetHasDirectMotion(false);
						r.y -= snapUp;
					}
				}
				else
				{
					// Check for step-down (ground is lower - slopes, small drops)
					// This keeps Sonic attached to the terrain instead of falling
					int snapDown = map->GetGroundSnapDownDistance(newGridRect);
					if (snapDown > 0)
					{
						// Before snapping down, verify we won't end up inside a horizontal wall
						// This prevents snapping into walls when ground ends at a wall boundary
						Rect snappedRect = { newGridRect.x, newGridRect.y + snapDown, newGridRect.w, newGridRect.h };

						// Probe if we can move even 1 pixel horizontally from the snapped position
						// If we can't move either direction, we'd be inside a wall
						int probeLeft = -1, probeRight = 1;
						int zeroDy = 0;
						Rect testRectLeft = snappedRect;
						Rect testRectRight = snappedRect;
						map->FilterGridMotion(testRectLeft, &probeLeft, &zeroDy);
						zeroDy = 0;
						map->FilterGridMotion(testRectRight, &probeRight, &zeroDy);

						// Only snap down if we can still move horizontally (not stuck inside a wall)
						bool insideWall = (probeLeft == 0 && probeRight == 0);
						if (!insideWall)
						{
							sprite->SetHasDirectMotion(true);
							sprite->Move(0, snapDown);
							sprite->SetHasDirectMotion(false);
							r.y += snapDown;
						}
					}
				}
			}

			// Safety check: push out of any wall overlap (catches edge cases)
			// Only apply when actually stuck (can't move either direction)
			Rect finalGridRect = { r.x, r.y - gridYOffset, r.w, r.h };
			int testLeft = -1, testRight = 1, zeroDy = 0;
			Rect testL = finalGridRect, testR = finalGridRect;
			map->FilterGridMotion(testL, &testLeft, &zeroDy);
			zeroDy = 0;
			map->FilterGridMotion(testR, &testRight, &zeroDy);

			// Only push out if truly stuck (can't move either direction)
			if (testLeft == 0 && testRight == 0)
			{
				int pushOut = map->GetWallPushOutDistance(finalGridRect);
				if (pushOut != 0)
				{
					sprite->SetHasDirectMotion(true);
					sprite->Move(pushOut, 0);
					sprite->SetHasDirectMotion(false);
					r.x += pushOut;
				}
			}
		}
	};
}
