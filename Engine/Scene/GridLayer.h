#pragma once

#include "Utils/Common.h"
#include "Rendering/Bitmap.h"

#include <vector>

namespace scene
{
	using namespace gfx;

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
		Dim totalRows = 0;
		Dim totalCols = 0;

		Dim tileWidth = 16;
		Dim tileHeight = 16;

		Dim gridElementWidth = 4;
		Dim gridElementHeight = 4;

		Dim SlopMaxElevetionPx = 32;
	};

	class GridMap 
	{
	private:
		friend class TileLayer;

	public:
		void Configure(GridConfig cfg);
		const GridConfig& Config() const;

		void FilterGridMotion(Rect& r, int* dx, int* dy, bool skipVertical = false);

		bool IsOnSolidGround(Rect& r);
		int GetGroundSnapDistance(Rect& r);
		int GetGroundSnapDownDistance(Rect& r);
		int GetWallPushOutDistance(Rect& r);  // Returns + for push right, - for push left, 0 if not in wall

		// Get the slope angle at a position (returns angle in degrees, positive = uphill going right)
		// Returns 0 if on flat ground, positive for upward slopes, negative for downward slopes
		float GetSlopeAngle(Rect& r, int sampleDistance = 8);

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

		bool LoadFromCSV(const std::string& csvContent);

		// RLE binary format - much faster than CSV for large grids
		bool LoadFromRLE(const std::string& filePath);
		bool SaveToRLE(const std::string& filePath) const;

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
		inline Dim GridBlockColumns();
		inline Dim GridBlockRows();
		inline Dim GridElementsPerTile();
		inline Dim DivGridElementWidth(Dim i);
		inline Dim DivGridElementHeight(Dim i);
		inline Dim MulGridElementWidth(Dim i);
		inline Dim MulGridElementHeight(Dim i);
		inline Dim GridBlockSizeof();
		inline bool Valid() const;

	private:
		GridConfig m_config{};
		std::vector<GridIndex> m_grid; 
	};
}
