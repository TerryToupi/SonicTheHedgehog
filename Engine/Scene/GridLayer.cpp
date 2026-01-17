#include "Scene/GridLayer.h"
#include "Utils/Assert.h"

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

	void GridMap::FilterGridMotion(Rect& r, int* dx, int* dy)
	{
		if (*dx < 0)
			FilterGridMotionLeft(r, dx);
		else if (*dx > 0)
			FilterGridMotionRight(r, dx);

		if (*dy < 0)
			FilterGridMotionUp(r, dy);
		else if (*dy > 0)
			FilterGridMotionDown(r, dy);
	}

	bool GridMap::IsOnSolidGround(Rect& r)
	{
		int dy = 1;
		FilterGridMotionDown(r, &dy);
		return dy == 0;
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

		if (y2_next >= m_config.totalRows * GridBlockRows())
			*dy = ((m_config.totalRows * GridBlockRows()) - 1) - y2;
		else 
		{
			auto newRow = DivGridElementHeight(y2_next);
			auto currRow = DivGridElementWidth(y2);

			if (newRow != currRow) 
			{
				auto startCol = DivGridElementWidth(r.x);
				auto endCol = DivGridElementWidth(r.x + r.w - 1);

				for (auto col = startCol; col <= endCol; ++col)
				{
					if (!CanPassGridTile(col, newRow, GRID_TOP_SOLID_MASK))
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

		if (x1_next < 0)
			*dx = -r.x;
		else 
		{
			auto newCol = DivGridElementWidth(x1_next);
			auto currCol = DivGridElementWidth(r.x);

			if (newCol != currCol) 
			{
				auto startRow = DivGridElementHeight(r.y);
				auto endRow = DivGridElementHeight(r.y + r.h - 1);

				for (auto row = startRow; row <= endRow; ++row)
				{
					if (!CanPassGridTile(newCol, row, GRID_RIGHT_SOLID_MASK)) 
					{
						*dx = DivGridElementWidth(currCol) - r.x;
						break;
					}
				}
			}
		}
	}

	void GridMap::FilterGridMotionRight(Rect& r, int* dx)
	{
		auto x2 = r.x + r.w - 1;
		auto x2_next = x2 + *dx;

		if (x2_next >= m_config.totalCols * GridBlockColumns())
			*dx = ((m_config.totalCols * GridBlockColumns()) - 1) - x2;
		else 
		{
			auto newCol = DivGridElementWidth(x2_next);
			auto currCol = DivGridElementWidth(x2);

			if (newCol != currCol) 
			{
				auto startRow = DivGridElementHeight(r.y);
				auto endRow = DivGridElementHeight(r.y + r.h - 1);

				for (auto row = startRow; row <= endRow; ++row)
				{
					if (!CanPassGridTile(newCol, row, GRID_LEFT_SOLID_MASK)) 
					{
						*dx = (MulGridElementWidth(newCol) - 1) - x2;
						break;
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
					if (!CanPassGridTile(col, newRow, GRID_BOTTOM_SOLID_MASK)) 
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
		// TODO: when the TileLayer exists

		//Bitmap tileElement;
		//tileElement.Generate(TILE_WIDTH, TILE_HEIGHT);
		//Bitmap gridElement;
		//gridElement.Generate(GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT);

		//auto index = tlayer->GetTile(col, row);
		//if (assumtedEmpty)
		//	SetGridTileBlockEmpty(col, row, tileCols);
		//else
		//{
		//	tlayer->PutTile(tileElement, 0, 0, tlayer->m_Tileset, index);
		//	ComputeGridBlock(
		//		GetGridTileBlock(col, row, tileCols),
		//		tileElement,
		//		gridElement,
		//		tlayer->m_Tileset,
		//		solidThreshold
		//	);
		//}
	}

	bool GridMap::ComputeIsGridIndexEmpty(Bitmap& gridElem, byte solidThreshold)
	{
		auto n = 0;
		BitmapAccessPixels(
			gridElem,
			[&n](PixelMemory mem)
			{
				RGBA memValue;
				ReadPixelColor(mem, &memValue);

				if (memValue.a != 0)
					++n;

				return true;
			}
		);

		return n <= solidThreshold;
	}

	void GridMap::ComputeGridBlock(GridIndex* block, Bitmap& tileElem, Bitmap& gridElem, Bitmap& tileSet, byte solidThreshold)
	{
		for (auto i = 0; i < GridElementsPerTile(); ++i)
		{
			auto x = i % GridBlockColumns();
			auto y = i / GridBlockRows();

			Rect tl = { 
				x * m_config.gridElementWidth, 
				y * m_config.gridElementHeight, 
				m_config.gridElementWidth, 
				m_config.gridElementHeight 
			};

			BitmapBlit(
				tileElem,
				tl,
				gridElem,
				{ 0, 0 }
			);

			auto isEmpty = ComputeIsGridIndexEmpty(gridElem, solidThreshold);
			*block++ = isEmpty ? GRID_EMPTY_TILE : GRID_SOLID_TILE;
		}
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
