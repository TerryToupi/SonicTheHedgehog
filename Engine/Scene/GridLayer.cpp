#include "Scene/GridLayer.h"
#include "Scene/TileLayer.h"
#include "Utils/Assert.h"

#include <sstream>
#include <string>
#include <fstream>
#include <cstdint>

namespace scene
{
	void GridMap::Configure(GridConfig cfg)
	{
		ASSERT(
			Valid(), 
			"FAILED. Configuration provaded for the grid has invalid size of tiles corespondent to the gird.\nMust me tileseize % gridElement == 0!"
		);

		m_config = cfg;
		m_grid.resize(
			(m_config.totalCols * m_config.totalRows * GridElementsPerTile()), 
			GRID_THIN_AIR_MASK
		);
	}

	const GridConfig& GridMap::Config() const
	{
		return m_config;
	}

	void GridMap::FilterGridMotion(Rect& r, int* dx, int* dy, bool skipVertical)
	{
		// Process horizontal movement first
		if (*dx < 0)
			FilterGridMotionLeft(r, dx);
		else if (*dx > 0)
			FilterGridMotionRight(r, dx);

		// Update rect position after horizontal filtering before checking vertical
		// This ensures vertical collision checks use the position after horizontal movement
		r.x += *dx;

		// Skip all vertical collision when flag is set (e.g., Sonic ascending in ball form)
		if (!skipVertical)
		{
			if (*dy < 0)
				FilterGridMotionUp(r, dy);
			else if (*dy > 0)
				FilterGridMotionDown(r, dy);
		}

		// Restore original x (caller expects unmodified rect, will apply dx separately)
		r.x -= *dx;
	}

	bool GridMap::IsOnSolidGround(Rect& r)
	{
		int dy = 1;
		FilterGridMotionDown(r, &dy);
		return dy == 0;
	}

	int GridMap::GetGroundSnapDistance(Rect& r)
	{
		// Check if sprite's bottom is inside any solid ground and return snap-up distance
		auto spriteBottom = r.y + r.h - 1;
		auto bottomRow = DivGridElementHeight(spriteBottom);
		auto startCol = DivGridElementWidth(r.x);
		auto endCol = DivGridElementWidth(r.x + r.w - 1);

		int maxSnapUp = 0;

		// Check the rows at the sprite's bottom for solid tiles
		// Look at the bottom few rows within step-up threshold
		auto maxRowsToCheck = m_config.SlopMaxElevetionPx / m_config.gridElementHeight;
		auto topRowToCheck = bottomRow > maxRowsToCheck ? bottomRow - maxRowsToCheck : 0;

		for (auto row = topRowToCheck; row <= bottomRow; ++row)
		{
			for (auto col = startCol; col <= endCol; ++col)
			{
				auto tileValue = GetGridTile(col, row);
				if (tileValue & GRID_TOP_SOLID_MASK)
				{
					// Found solid ground - calculate snap distance
					// Ground surface is at the top of this row
					auto groundSurface = MulGridElementHeight(row);
					auto snapUp = (spriteBottom + 1) - groundSurface;
					if (snapUp > 0 && snapUp > maxSnapUp)
					{
						maxSnapUp = snapUp;
					}
				}
			}
		}

		// Only return snap distance if within step-up threshold
		if (maxSnapUp <= static_cast<int>(m_config.SlopMaxElevetionPx))
		{
			return maxSnapUp;
		}
		return 0;
	}

	int GridMap::GetGroundSnapDownDistance(Rect& r)
	{
		// Find ground below sprite and return snap-down distance (for stepping down)
		auto spriteBottom = r.y + r.h - 1;
		auto bottomRow = DivGridElementHeight(spriteBottom);
		auto startCol = DivGridElementWidth(r.x);
		auto endCol = DivGridElementWidth(r.x + r.w - 1);

		// Check if already on solid ground - no snap down needed
		for (auto col = startCol; col <= endCol; ++col)
		{
			if (GetGridTile(col, bottomRow + 1) & GRID_TOP_SOLID_MASK)
			{
				return 0; // Already on ground
			}
		}

		// Search downward within step-down threshold for ground
		auto maxRowsToCheck = m_config.SlopMaxElevetionPx / m_config.gridElementHeight;
		auto gridMaxRows = m_config.totalRows * (m_config.tileHeight / m_config.gridElementHeight);

		for (Dim offset = 1; offset <= maxRowsToCheck; ++offset)
		{
			auto checkRow = bottomRow + 1 + offset;
			if (checkRow >= gridMaxRows)
				break;

			for (auto col = startCol; col <= endCol; ++col)
			{
				if (GetGridTile(col, checkRow) & GRID_TOP_SOLID_MASK)
				{
					// Found ground below - calculate snap distance
					auto groundSurface = MulGridElementHeight(checkRow);
					auto snapDown = groundSurface - (spriteBottom + 1);
					return snapDown;
				}
			}
		}

		return 0; // No ground found within threshold
	}

	int GridMap::GetWallPushOutDistance(Rect& r)
	{
		// Check if sprite is stuck inside a wall (solid tile at body level, not just feet)
		// Only triggers when sprite body overlaps solid and can't move either direction
		// Returns positive to push right, negative to push left, 0 if not stuck

		auto spriteLeft = r.x;
		auto spriteRight = r.x + r.w - 1;
		auto spriteTop = r.y;

		// Check upper body area only (exclude feet/step-up area)
		auto bodyCheckBottom = r.y + r.h - 1 - m_config.SlopMaxElevetionPx;
		if (bodyCheckBottom <= spriteTop) return 0;  // Sprite too small

		auto leftCol = DivGridElementWidth(spriteLeft);
		auto rightCol = DivGridElementWidth(spriteRight);
		auto topRow = DivGridElementHeight(spriteTop);
		auto bodyBottomRow = DivGridElementHeight(bodyCheckBottom);

		// Bounds check
		if (leftCol < 0) leftCol = 0;
		if (rightCol < 0) return 0;
		if (topRow < 0) topRow = 0;

		// Check right edge for wall overlap in body area
		for (Dim col = rightCol; col >= leftCol && col >= 0; --col)
		{
			for (Dim row = topRow; row <= bodyBottomRow; ++row)
			{
				if (GetGridTile(col, row) & GRID_SOLID_TILE)
				{
					// Body overlaps solid - push left
					auto pushDistance = spriteRight - MulGridElementWidth(col) + 1;
					if (pushDistance > 0 && pushDistance <= 8)  // Max 8 pixel push
					{
						return -pushDistance;
					}
				}
			}
		}

		// Check left edge for wall overlap in body area
		for (Dim col = leftCol; col <= rightCol; ++col)
		{
			for (Dim row = topRow; row <= bodyBottomRow; ++row)
			{
				if (GetGridTile(col, row) & GRID_SOLID_TILE)
				{
					// Body overlaps solid - push right
					auto pushDistance = MulGridElementWidth(col + 1) - spriteLeft;
					if (pushDistance > 0 && pushDistance <= 8)  // Max 8 pixel push
					{
						return pushDistance;
					}
				}
			}
		}

		return 0;
	}

	GridIndex* GridMap::Data(void)
	{
		return static_cast<GridIndex*>(m_grid.data());
	}

	void GridMap::SetGridTile(Dim col, Dim row, GridIndex index)
	{
		Dim totalCols = m_config.totalCols * GridBlockColumns();
		m_grid[row * totalCols + col] = index;
	}

	GridIndex GridMap::GetGridTile(Dim col, Dim row)
	{
		Dim totalCols = m_config.totalCols * GridBlockColumns();
		Dim totalRows = m_config.totalRows * GridBlockRows();

		// Bounds checking to prevent crashes
		if (col < 0 || col >= totalCols || row < 0 || row >= totalRows)
			return GRID_EMPTY_TILE;  // Out of bounds = empty/passable

		return m_grid[row * totalCols + col];
	}

	void GridMap::SetSolidGridTile(Dim col, Dim row)
	{
		SetGridTile(col, row, GRID_SOLID_TILE);
	}

	void GridMap::SetEmptyGridTile(Dim col, Dim row)
	{
		SetGridTile(col, row, GRID_EMPTY_TILE);
	}

	void GridMap::SetGridTileFlags(Dim col, Dim row, GridIndex flags)
	{
		SetGridTile(col, row, flags);
	}

	void GridMap::SetGridTileTopSolidOnly(Dim col, Dim row)
	{
		SetGridTile(row, col, GRID_TOP_SOLID_MASK);
	}

	bool GridMap::CanPassGridTile(Dim col, Dim row, GridIndex flags)
	{
		return (GetGridTile(col, row) & flags) != 0;
	}

	GridIndex* GridMap::GetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols)
	{
		Dim pos = (rowTile * tileCols + colTile) * GridElementsPerTile();
		return m_grid.data() + pos;
	}

	void GridMap::SetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols, GridIndex flags)
	{
		GridIndex* block = GetGridTileBlock(colTile, rowTile, tileCols);
		std::fill_n(block, GridElementsPerTile(), flags);
	}

	void GridMap::FilterGridMotionDown(Rect& r, int* dy)
	{
		auto y2 = r.y + r.h - 1;
		auto y2_next = y2 + *dy;

		// Grid max in PIXELS = totalRows * tileHeight (not grid elements!)
		auto gridMaxPixels = m_config.totalRows * m_config.tileHeight;

		if (y2_next >= gridMaxPixels)
		{
			*dy = (gridMaxPixels - 1) - y2;
		}
		else
		{
			auto newRow = DivGridElementHeight(y2_next);
			auto currRow = DivGridElementHeight(y2);

			if (newRow != currRow)
			{
				auto startCol = DivGridElementWidth(r.x);
				auto endCol = DivGridElementWidth(r.x + r.w - 1);

				for (auto col = startCol; col <= endCol; ++col)
				{
					auto tileValue = GetGridTile(col, newRow);
					// Block when tile IS solid (has the flag)
					if (tileValue & GRID_TOP_SOLID_MASK)
					{
						*dy = (MulGridElementHeight(newRow) - 1) - y2;
						break;
					}
				}
			}
		}
	}

	void GridMap::FilterGridMotionLeft(Rect& r, int* dx)
	{
		auto x1_next = r.x + *dx;

		// Clamp to left boundary
		if (x1_next < 0)
		{
			*dx = -r.x;
			return;  // At left edge, no further checks needed
		}

		auto newCol = DivGridElementWidth(x1_next);
		auto currCol = DivGridElementWidth(r.x);

		// Only check if crossing column boundary and columns are valid
		if (newCol != currCol && currCol >= 0 && newCol >= 0)
		{
			auto startRow = DivGridElementHeight(r.y);
			auto endRow = DivGridElementHeight(r.y + r.h - 1);
			auto spriteBottom = r.y + r.h;

			// Calculate step threshold row
			auto stepThresholdY = spriteBottom - m_config.SlopMaxElevetionPx;
			auto stepThresholdRow = DivGridElementHeight(stepThresholdY);

			// Ensure we start from a valid column (use signed int for proper comparison)
			int startCheckCol = (currCol > 0) ? static_cast<int>(currCol) - 1 : 0;
			int endCheckCol = static_cast<int>(newCol);

			// Check ALL columns being traversed (from right to left, since moving left)
			// Use signed int to handle the case when checkCol reaches 0 and decrements
			for (int checkCol = startCheckCol; checkCol >= endCheckCol && checkCol >= 0; --checkCol)
			{
					// Find the FIRST (topmost) collision at this column
					int groundSurfaceRow = -1;
					bool hasUpperCollision = false;

					for (auto row = startRow; row <= endRow; ++row)
					{
						auto tile = GetGridTile(checkCol, row);
						if (tile & GRID_SOLID_TILE)
						{
							if (groundSurfaceRow == -1)
							{
								groundSurfaceRow = row;
							}

							if (row < stepThresholdRow)
							{
								hasUpperCollision = true;
							}
						}
					}

					if (groundSurfaceRow != -1)
					{
						if (hasUpperCollision)
						{
							// Wall at this column - block at this column
							*dx = MulGridElementWidth(checkCol + 1) - r.x;
							return;
						}
						else
						{
							auto groundSurfaceY = MulGridElementHeight(groundSurfaceRow);
							auto stepUpNeeded = spriteBottom - groundSurfaceY;

							if (stepUpNeeded <= 0)
							{
								// Ground at or below feet - continue checking next columns
							}
							else if (stepUpNeeded <= static_cast<int>(m_config.SlopMaxElevetionPx))
							{
								// Check air space above ground for walls
								auto steppedUpY = r.y - stepUpNeeded;
								auto steppedStartRow = DivGridElementHeight(steppedUpY);

								for (auto row = steppedStartRow; row < groundSurfaceRow; ++row)
								{
									if (GetGridTile(checkCol, row) & GRID_SOLID_TILE)
									{
										// Wall above ground - block at this column
										*dx = MulGridElementWidth(checkCol + 1) - r.x;
										return;
									}
								}
								// Path clear at this column, continue to next
							}
							else
							{
								// Step too high - block at this column
								*dx = MulGridElementWidth(checkCol + 1) - r.x;
								return;
							}
						}
					}
				}
			}
		}

	void GridMap::FilterGridMotionRight(Rect& r, int* dx)
	{
		auto x2 = r.x + r.w - 1;
		auto x2_next = x2 + *dx;

		// Grid max in PIXELS
		auto gridMaxPixels = m_config.totalCols * m_config.tileWidth;

		if (x2_next >= gridMaxPixels)
			*dx = (gridMaxPixels - 1) - x2;
		else
		{
			auto newCol = DivGridElementWidth(x2_next);
			auto currCol = DivGridElementWidth(x2);

			if (newCol != currCol)
			{
				auto startRow = DivGridElementHeight(r.y);
				auto endRow = DivGridElementHeight(r.y + r.h - 1);
				auto spriteBottom = r.y + r.h;

				// Calculate step threshold row
				auto stepThresholdY = spriteBottom - m_config.SlopMaxElevetionPx;
				auto stepThresholdRow = DivGridElementHeight(stepThresholdY);

				// Check ALL columns being traversed (important for 1x1 grid with multi-pixel movement)
				for (auto checkCol = currCol + 1; checkCol <= newCol; ++checkCol)
				{
					// Find the FIRST (topmost) collision at this column - this is the ground surface
					int groundSurfaceRow = -1;
					bool hasUpperCollision = false;

					for (auto row = startRow; row <= endRow; ++row)
					{
						auto tile = GetGridTile(checkCol, row);
						if (tile & GRID_SOLID_TILE)
						{
							if (groundSurfaceRow == -1)
							{
								groundSurfaceRow = row;
							}

							if (row < stepThresholdRow)
							{
								hasUpperCollision = true;
							}
						}
					}

					if (groundSurfaceRow != -1)
					{
						if (hasUpperCollision)
						{
							// Wall at this column - block at this column
							*dx = (MulGridElementWidth(checkCol) - 1) - x2;
							return;
						}
						else
						{
							auto groundSurfaceY = MulGridElementHeight(groundSurfaceRow);
							auto stepUpNeeded = spriteBottom - groundSurfaceY;

							if (stepUpNeeded <= 0)
							{
								// Ground at or below feet - continue checking next columns
							}
							else if (stepUpNeeded <= static_cast<int>(m_config.SlopMaxElevetionPx))
							{
								// Check air space above ground for walls
								auto steppedUpY = r.y - stepUpNeeded;
								auto steppedStartRow = DivGridElementHeight(steppedUpY);

								for (auto row = steppedStartRow; row < groundSurfaceRow; ++row)
								{
									if (GetGridTile(checkCol, row) & GRID_SOLID_TILE)
									{
										// Wall above ground - block at this column
										*dx = (MulGridElementWidth(checkCol) - 1) - x2;
										return;
									}
								}
								// Path clear at this column, continue to next
							}
							else
							{
								// Step too high - block at this column
								*dx = (MulGridElementWidth(checkCol) - 1) - x2;
								return;
							}
						}
					}
				}
			}
		}
	}

	void GridMap::FilterGridMotionUp(Rect& r, int* dy)
	{
		auto y1_next = r.y + *dy;
		if (y1_next < 0)
			*dy = -r.y;
		else 
		{
			auto newRow = DivGridElementHeight(y1_next);
			auto currRow = DivGridElementWidth(r.y);

			if (newRow != currRow) 
			{

				auto startCol = DivGridElementWidth(r.x);
				auto endCol = DivGridElementWidth(r.x + r.w - 1);

				for (auto col = startCol; col <= endCol; ++col)
				{
					// Block when tile IS solid (has the flag)
					if (CanPassGridTile(col, newRow, GRID_BOTTOM_SOLID_MASK))
					{
						*dy = MulGridElementHeight(currRow) - r.y;
						break;
					}
				}
			}
		}
	}

	void GridMap::ComputeTileGridBlock(TileLayer* tlayer, Dim row, Dim col, Dim tileCols, byte solidThreshold, bool assumtedEmpty)
	{ 
		// Bitmap tileElement = BitmapCreate(m_config.tileWidth, m_config.tileHeight);
		// Bitmap gridElement = BitmapCreate(m_config.gridElementWidth, m_config.gridElementHeight);

		// auto index = tlayer->GetTile(col, row);
		// if (assumtedEmpty)
		// 	SetGridTileBlock(col, row, tileCols, GRID_EMPTY_TILE);
		// else
		// {
		// 	tlayer->PutTile(tileElement, 0, 0, tlayer->m_tileset, index);
		// 	ComputeGridBlock(
		// 		GetGridTileBlock(col, row, tileCols),
		// 		tileElement,
		// 		gridElement,
		// 		tlayer->m_tileset,
		// 		solidThreshold
		// 	);
		// }

		// BitmapDestroy(tileElement);
		// BitmapDestroy(gridElement);
	}

	bool GridMap::LoadFromCSV(const std::string& csvContent)
	{
		if (m_grid.empty())
			return false; // Grid must be configured first

		std::istringstream stream(csvContent);
		std::string line;
		Dim row = 0;
		Dim totalCols = m_config.totalCols * GridBlockColumns();
		Dim totalRows = m_config.totalRows * GridBlockRows();

		while (std::getline(stream, line) && row < totalRows)
		{
			if (line.empty())
				continue;

			std::istringstream lineStream(line);
			std::string cell;
			Dim col = 0;

			while (std::getline(lineStream, cell, ',') && col < totalCols)
			{
				int value = std::stoi(cell);
				GridIndex gridValue;

				// Map CSV values to grid flags
				if (value == -1)
					gridValue = GRID_EMPTY_TILE;
				else if (value == 0)
					gridValue = GRID_SOLID_TILE;
				else
					gridValue = static_cast<GridIndex>(value);

				m_grid[row * totalCols + col] = gridValue;
				++col;
			}

			if (col != totalCols)
				return false; // Column count mismatch

			++row;
		}

		if (row != totalRows)
			return false; // Row count mismatch

		return true;
	}

	bool GridMap::LoadFromRLE(const std::string& filePath)
	{
		if (m_grid.empty())
			return false; // Grid must be configured first

		std::ifstream file(filePath, std::ios::binary);
		if (!file)
			return false;

		// Read and verify header
		char magic[4];
		file.read(magic, 4);
		if (magic[0] != 'G' || magic[1] != 'R' || magic[2] != 'L' || magic[3] != 'E')
			return false; // Invalid magic

		uint32_t version, cols, rows;
		file.read(reinterpret_cast<char*>(&version), sizeof(version));
		file.read(reinterpret_cast<char*>(&cols), sizeof(cols));
		file.read(reinterpret_cast<char*>(&rows), sizeof(rows));

		if (version != 1)
			return false; // Unsupported version

		Dim totalCols = m_config.totalCols * GridBlockColumns();
		Dim totalRows = m_config.totalRows * GridBlockRows();

		if (cols != totalCols || rows != totalRows)
			return false; // Size mismatch

		// Read RLE data
		size_t index = 0;
		size_t totalCells = static_cast<size_t>(totalCols) * totalRows;

		while (index < totalCells && file)
		{
			uint8_t value;
			uint32_t count;

			file.read(reinterpret_cast<char*>(&value), sizeof(value));
			file.read(reinterpret_cast<char*>(&count), sizeof(count));

			if (!file)
				break;

			// Fill the grid with this run
			GridIndex gridValue = static_cast<GridIndex>(value);
			size_t endIndex = std::min(index + count, totalCells);

			for (size_t i = index; i < endIndex; ++i)
				m_grid[i] = gridValue;

			index = endIndex;
		}

		return index == totalCells;
	}

	bool GridMap::SaveToRLE(const std::string& filePath) const
	{
		if (m_grid.empty())
			return false;

		std::ofstream file(filePath, std::ios::binary);
		if (!file)
			return false;

		// Write header
		file.write("GRLE", 4); // Magic

		uint32_t version = 1;
		uint32_t cols = m_config.totalCols * (m_config.tileWidth / m_config.gridElementWidth);
		uint32_t rows = m_config.totalRows * (m_config.tileHeight / m_config.gridElementHeight);

		file.write(reinterpret_cast<const char*>(&version), sizeof(version));
		file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
		file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));

		// Write RLE data
		if (m_grid.empty())
			return true;

		uint8_t currentValue = static_cast<uint8_t>(m_grid[0]);
		uint32_t count = 1;

		for (size_t i = 1; i < m_grid.size(); ++i)
		{
			uint8_t value = static_cast<uint8_t>(m_grid[i]);

			if (value == currentValue && count < UINT32_MAX)
			{
				++count;
			}
			else
			{
				// Write the run
				file.write(reinterpret_cast<const char*>(&currentValue), sizeof(currentValue));
				file.write(reinterpret_cast<const char*>(&count), sizeof(count));

				currentValue = value;
				count = 1;
			}
		}

		// Write final run
		file.write(reinterpret_cast<const char*>(&currentValue), sizeof(currentValue));
		file.write(reinterpret_cast<const char*>(&count), sizeof(count));

		return file.good();
	}

	bool GridMap::ComputeIsGridIndexEmpty(Bitmap& gridElem, byte solidThreshold)
	{
		// auto n = 0;
		// BitmapAccessPixels(
		// 	gridElem,
		// 	[&n](PixelMemory mem)
		// 	{
		// 		RGBA memValue;
		// 		ReadPixelColor(mem, &memValue);

		// 		if (memValue.a != 0)
		// 			++n;

		// 		return true;
		// 	}
		// );

		// return n <= solidThreshold;
	}

	void GridMap::ComputeGridBlock(GridIndex* block, Bitmap& tileElem, Bitmap& gridElem, Bitmap& tileSet, byte solidThreshold)
	{
		// for (auto i = 0; i < GridElementsPerTile(); ++i)
		// {
		// 	auto x = i % GridBlockColumns();
		// 	auto y = i / GridBlockRows();

		// 	Rect tl = { 
		// 		x * m_config.gridElementWidth, 
		// 		y * m_config.gridElementHeight, 
		// 		m_config.gridElementWidth, 
		// 		m_config.gridElementHeight 
		// 	};

		// 	BitmapBlit(
		// 		tileElem,
		// 		tl,
		// 		gridElem,
		// 		{ 0, 0 }
		// 	);

		// 	auto isEmpty = ComputeIsGridIndexEmpty(gridElem, solidThreshold);
		// 	*block++ = isEmpty ? GRID_EMPTY_TILE : GRID_SOLID_TILE;
		// }
	}

	inline Dim GridMap::GridBlockColumns()
	{
		return m_config.tileWidth / m_config.gridElementWidth;
	}

	inline Dim GridMap::GridBlockRows()
	{
		return m_config.tileHeight / m_config.gridElementHeight;
	}

	inline Dim GridMap::GridElementsPerTile()
	{
		return GridBlockColumns() * GridBlockRows();
	}

	inline Dim GridMap::DivGridElementWidth(Dim i)
	{
		return i / m_config.gridElementWidth;
	}

	inline Dim GridMap::DivGridElementHeight(Dim i)
	{
		return i / m_config.gridElementHeight;
	}

	inline Dim GridMap::MulGridElementWidth(Dim i)
	{
		return i * m_config.gridElementWidth;
	}

	inline Dim GridMap::MulGridElementHeight(Dim i)
	{
		return i * m_config.gridElementHeight;
	}

	inline Dim GridMap::GridBlockSizeof()
	{
		return GridElementsPerTile() * sizeof(GridIndex);
	}

	inline bool GridMap::Valid() const
	{
		return (m_config.tileHeight % m_config.gridElementHeight) == 0 && (m_config.tileWidth % m_config.gridElementWidth) == 0;
	}
}
