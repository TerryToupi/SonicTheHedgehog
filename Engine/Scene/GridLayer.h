#pragma once

#include "Utils/Common.h"
#include "Rendering/Bitmap.h"
#include "Core/LatelyDestroyable.h"

#include <vector>

namespace scene
{
	using namespace gfx;
	using namespace core;

	class TileLayer;

	inline constexpr unsigned short GRID_THIN_AIR_MASK = 0x00; // element is ignored
	inline constexpr unsigned short GRID_LEFT_SOLID_MASK = 0x01; // bit 0
	inline constexpr unsigned short GRID_RIGHT_SOLID_MASK = 0x02; // bit 1
	inline constexpr unsigned short GRID_TOP_SOLID_MASK = 0x04; // bit 2
	inline constexpr unsigned short GRID_BOTTOM_SOLID_MASK = 0x08; // bit 3
	inline constexpr unsigned short GRID_GROUND_MASK = 0x10; // bit 4
	inline constexpr unsigned short GRID_FLOATING_MASK = 0x20; // bit 5
	inline constexpr unsigned short GRID_EMPTY_TILE = GRID_THIN_AIR_MASK;
	inline constexpr unsigned short GRID_SOLID_TILE =
		(GRID_LEFT_SOLID_MASK | GRID_RIGHT_SOLID_MASK | GRID_TOP_SOLID_MASK | GRID_BOTTOM_SOLID_MASK);

	struct GridConfig
	{
		Dim tileWidth = 16;
		Dim tileHeight = 16;

		Dim gridElementWidth = 4;
		Dim gridElementHeight = 4;

		Dim SlopMaxElevetionPx = 32;

		Dim totalRows = 0;
		Dim totalCols = 0;

		Dim GridBlockColumns();
		Dim GridBlockRows();
		Dim GridElementsPerTile();

		Dim DivGridElementWidth(Dim i);
		Dim DivGridElementHeight(Dim i);
		Dim MulGridElementWidth(Dim i);
		Dim MulGridElementHeight(Dim i);

		Dim GridBlockSizeof();

		bool Valid() const;
	};

	class GridMap : public LatelyDestroyable
	{
	public:
		void Configure(GridConfig cfg);
		const GridConfig& Config() const;

		void FilterGridMotion(Rect& r, int* dx, int* dy);

		bool IsOnSolidGround(Rect& r);

		GridIndex* Data(void);
		void SetGridTile(Dim col, Dim row, GridIndex index);
		GridIndex GetGridTile(Dim col, Dim row);

		void SetSolidGridTile(Dim col, Dim row);
		void SetEmptyGridTile(Dim col, Dim row);
		void SetGridTileFlags(Dim col, Dim row, GridIndex flags);
		void SetGridTileTopSolidOnly(Dim col, Dim row);
		bool CanPassGridTile(Dim col, Dim row, GridIndex flags);

		GridIndex* GetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols);
		void SetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols, GridIndex flags);

		void ComputeTileGridBlock(TileLayer* tlayer, Dim row, Dim col, Dim tileCols, byte solidThreshold, bool assumtedEmpty);

		GridMap() = default;
		~GridMap() = default;

	private:
		void FilterGridMotionDown(Rect& r, int* dy);
		void FilterGridMotionLeft(Rect& r, int* dx);
		void FilterGridMotionRight(Rect& r, int* dx);
		void FilterGridMotionUp(Rect& r, int* dy);

		bool ComputeIsGridIndexEmpty(Bitmap& gridElem, byte solidThreshold);
		void ComputeGridBlock(GridIndex* block, Bitmap& tileElem, Bitmap& gridElem, Bitmap& tileSet, byte solidThreshold);

	private:
		GridConfig m_config{};
		std::vector<GridIndex> m_grid; 
	};
}
