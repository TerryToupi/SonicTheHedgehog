#include "Scene/GridLayer.h"
#include "Utils/Assert.h"

namespace scene
{
	Dim GridConfig::GridBlockColumns()
	{
		return tileWidth / gridElementWidth;
	}

	Dim GridConfig::GridBlockRows()
	{
		return tileHeight / gridElementHeight;
	}

	Dim GridConfig::GridElementsPerTile()
	{
		return GridBlockColumns() * GridBlockRows();
	}

	Dim GridConfig::DivGridElementWidth(Dim i)
	{
		return i / gridElementWidth;
	}

	Dim GridConfig::DivGridElementHeight(Dim i)
	{
		return i / gridElementHeight;
	}

	Dim GridConfig::MulGridElementWidth(Dim i)
	{
		return i * gridElementWidth;
	}

	Dim GridConfig::MulGridElementHeight(Dim i)
	{
		return i * gridElementHeight;
	}

	Dim GridConfig::GridBlockSizeof()
	{
		return GridElementsPerTile() * sizeof(GridIndex);
	}

	bool GridConfig::Valid() const
	{
		return (tileHeight % gridElementHeight) == 0 && (tileWidth % gridElementWidth) == 0;
	}

	void GridMap::Configure(GridConfig cfg)
	{
		ASSERT(
			cfg.Valid(), 
			"FAILED. Configuration provaded for the grid has invalid size of tiles corespondent to the gird.\nMust me tileseize % gridElement == 0!"
		);

		m_config = cfg;
		m_grid.resize(
			(m_config.totalCols * m_config.totalRows * m_config.GridElementsPerTile()), 
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
		Dim totalCols = m_config.totalCols * m_config.GridBlockColumns();
		m_grid[row * totalCols + col] = index;
	}

	GridIndex GridMap::GetGridTile(Dim col, Dim row)
	{
		Dim totalCols = m_config.totalCols * m_config.GridBlockColumns();
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
		Dim pos = (rowTile * tileCols + colTile) * m_config.GridElementsPerTile();
		return m_grid.data() + pos;
	}

	void GridMap::SetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols, GridIndex flags)
	{
		GridIndex* block = GetGridTileBlock(colTile, rowTile, tileCols);
		std::fill_n(block, m_config.GridElementsPerTile(), flags);
	}

	void GridMap::FilterGridMotionDown(Rect& r, int* dy)
	{
		auto y2 = r.y + r.h - 1;
		auto y2_next = y2 + *dy;

		if (y2_next >= m_config.totalRows * m_config.GridBlockRows())
			*dy = ((m_config.totalRows * m_config.GridBlockRows()) - 1) - y2;
		else 
		{
			auto newRow = m_config.DivGridElementHeight(y2_next);
			auto currRow = m_config.DivGridElementWidth(y2);

			if (newRow != currRow) 
			{
				auto startCol = m_config.DivGridElementWidth(r.x);
				auto endCol = m_config.DivGridElementWidth(r.x + r.w - 1);

				for (auto col = startCol; col <= endCol; ++col)
				{
					if (!CanPassGridTile(col, newRow, GRID_TOP_SOLID_MASK))
					{
						*dy = (m_config.MulGridElementHeight(newRow) - 1) - y2;
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
			auto newCol = m_config.DivGridElementWidth(x1_next);
			auto currCol = m_config.DivGridElementWidth(r.x);

			if (newCol != currCol) 
			{
				auto startRow = m_config.DivGridElementHeight(r.y);
				auto endRow = m_config.DivGridElementHeight(r.y + r.h - 1);

				for (auto row = startRow; row <= endRow; ++row)
				{
					if (!CanPassGridTile(newCol, row, GRID_RIGHT_SOLID_MASK)) 
					{
						*dx = m_config.DivGridElementWidth(currCol) - r.x;
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

		if (x2_next >= m_config.totalCols * m_config.GridBlockColumns())
			*dx = ((m_config.totalCols * m_config.GridBlockColumns()) - 1) - x2;
		else 
		{
			auto newCol = m_config.DivGridElementWidth(x2_next);
			auto currCol = m_config.DivGridElementWidth(x2);

			if (newCol != currCol) 
			{
				auto startRow = m_config.DivGridElementHeight(r.y);
				auto endRow = m_config.DivGridElementHeight(r.y + r.h - 1);

				for (auto row = startRow; row <= endRow; ++row)
				{
					if (!CanPassGridTile(newCol, row, GRID_LEFT_SOLID_MASK)) 
					{
						*dx = (m_config.MulGridElementWidth(newCol) - 1) - x2;
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
			auto newRow = m_config.DivGridElementHeight(y1_next);
			auto currRow = m_config.DivGridElementWidth(r.y);

			if (newRow != currRow) 
			{

				auto startCol = m_config.DivGridElementWidth(r.x);
				auto endCol = m_config.DivGridElementWidth(r.x + r.w - 1);

				for (auto col = startCol; col <= endCol; ++col)
				{
					if (!CanPassGridTile(col, newRow, GRID_BOTTOM_SOLID_MASK)) 
					{
						*dy = m_config.MulGridElementHeight(currRow) - r.y;
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
		//TODO figure out the transparent color

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
		for (auto i = 0; i < m_config.GridElementsPerTile(); ++i)
		{
			auto x = i % m_config.GridBlockColumns();
			auto y = i / m_config.GridBlockRows();

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
}
