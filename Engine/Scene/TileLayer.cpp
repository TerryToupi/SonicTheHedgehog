#include "Scene/TileLayer.h"

#include <sstream>
#include <string>

namespace scene
{
	TileLayer::~TileLayer()
	{
		if (m_tileset)
			BitmapDestroy(m_tileset);
		if (m_dpyBuffer)
			BitmapDestroy(m_dpyBuffer);
	}

	inline Index TileLayer::DivTileWidth(Index i)
	{
		return Index(i / m_config.tileWidth);
	}

	inline Index TileLayer::DivTileHeight(Index i)
	{
		return Index(i / m_config.tileHeight);
	}

	inline Index TileLayer::MulTileHeight(Index i)
	{
		return Index(i * m_config.tileHeight);
	}

	inline Index TileLayer::MulTileWidth(Index i)
	{
		return Index(i * m_config.tileWidth);
	}

	inline Index TileLayer::ModTileWidth(Index i)
	{
		return Index(i % m_config.tileWidth);
	}

	inline Index TileLayer::ModTileHeight(Index i)
	{
		return Index(i % m_config.tileHeight);
	}

	void TileLayer::Configure(TileConfig& cfg)
	{
		m_config = cfg;
		m_map.resize(m_config.totalCols * m_config.totalRows, MakeIndex(UINT16_MAX, UINT16_MAX));

		// Buffer must fit all potentially visible tiles (viewport may span partial tiles)
		Dim bufferCols = (m_config.viewWindow.w / m_config.tileWidth) + 2;
		Dim bufferRows = (m_config.viewWindow.h / m_config.tileHeight) + 2;
		m_dpyBuffer = BitmapCreate(bufferCols * m_config.tileWidth, bufferRows * m_config.tileHeight);
	}

	const TileConfig& TileLayer::Config(void)
	{
		return m_config;
	}

	void TileLayer::SetTile(Dim col, Dim row, Index index)
	{
		m_map[row * m_config.totalCols + col] = index;
	}

	Index TileLayer::GetTile(Dim col, Dim row) const
	{
		// Bounds check - return empty tile if out of range
		if (col >= m_config.totalCols || row >= m_config.totalRows)
			return MakeIndex(UINT16_MAX, UINT16_MAX);
		return m_map[row * m_config.totalCols + col];
	}

	const Point TileLayer::Pick(Dim x, Dim y) 
	{
		Index ix = DivTileWidth(x + m_config.viewWindow.x);
		Index iy = DivTileWidth(y + m_config.viewWindow.y);
		return { (int)ix, (int)iy };
	}

	void TileLayer::PutTile(Bitmap& dest, Dim x, Dim y, Bitmap& tiles, Index tile)
	{
		// Skip empty tiles
		if (TileX(tile) == UINT16_MAX && TileY(tile) == UINT16_MAX)
			return;

		BitmapBlit(
			m_tileset,
			{ TileX(tile), TileY(tile), m_config.tileWidth, m_config.tileHeight },
			dest,
			{ x, y }
		);
	}

	/*static*/ Dim TileLayer::TileY(Index index)
	{
		return Dim(index >> 16);
	}

	/*static*/ Dim TileLayer::TileX(Index index)
	{
		return Dim(index & 0xFFFF);
	}

	/*static*/ Index TileLayer::MakeIndex(Dim row, Dim col)
	{
		return (Index(row) << 16) | Index(col);
	}

	const Rect& TileLayer::GetViewWindow(void)
	{
		return m_config.viewWindow;
	}

	void TileLayer::SetViewWindow(const Rect& r)
	{
		m_config.viewWindow = r;
		m_dpyChanged = true;
	}

	void TileLayer::Display(Bitmap& dest, const Point& dp)
	{
		if (m_dpyChanged)
		{
			auto startCol = DivTileWidth(m_config.viewWindow.x);
			auto startRow = DivTileHeight(m_config.viewWindow.y);
			auto endCol = DivTileWidth(m_config.viewWindow.x + m_config.viewWindow.w - 1);
			auto endRow = DivTileHeight(m_config.viewWindow.y + m_config.viewWindow.h - 1);
			m_dpyX = ModTileWidth(m_config.viewWindow.x);
			m_dpyY = ModTileHeight(m_config.viewWindow.y);
			m_dpyChanged = false;

			// Clear buffer before rendering (transparent for empty tiles)
			BitmapClear(m_dpyBuffer, MakeColor(0, 0, 0, 0));

			for (Dim row = startRow; row <= endRow; ++row)
				for (Dim col = startCol; col <= endCol; ++col)
					PutTile(
						m_dpyBuffer,
						MulTileWidth(col - startCol),
						MulTileHeight(row - startRow),
						m_tileset,
						GetTile(col, row)  // Fixed: was GetTile(row, col)
					);
		}

		BitmapBlit(
			m_dpyBuffer,
			{ m_dpyX, m_dpyY, m_config.viewWindow.w, m_config.viewWindow.h },
			dest,
			{ dp.x, dp.y }
		);
	}

	Bitmap TileLayer::GetBitmap(void) const
	{
		return m_dpyBuffer;
	}

	int TileLayer::GetPixelWidth(void) const
	{
		return m_config.viewWindow.w;
	}

	int TileLayer::GetPixelHeight(void) const
	{
		return m_config.viewWindow.h;
	}

	unsigned TileLayer::GetTileWidth(void) 
	{
		return DivTileWidth(m_config.viewWindow.w);
	}

	unsigned TileLayer::GetTileHeight(void)
	{
		return DivTileHeight(m_config.viewWindow.h);
	}

	void TileLayer::Scroll(float dx, float dy)
	{
		m_config.viewWindow.x += dx;
		m_config.viewWindow.y += dy;
		m_dpyChanged = true;
	}

	void TileLayer::FilterScrollDistance(int viewStartCoord, int viewSize, int* d, int maxMapSize)
	{
		auto val = *d + viewStartCoord;
		if (val < 0)
			*d = viewStartCoord;

		else if (viewSize >= maxMapSize)
			*d = 0;

		else if ((val + viewSize) >= maxMapSize)
			*d = maxMapSize - (viewStartCoord + viewSize);
	}

	void TileLayer::FilterScroll(int* dx, int* dy)
	{
		FilterScrollDistance(m_config.viewWindow.x, m_config.viewWindow.w, dx, GetPixelWidth());
		FilterScrollDistance(m_config.viewWindow.y, m_config.viewWindow.h, dy, GetPixelHeight());
	}

	void TileLayer::ScrollWithBoundsCheck(int _dx, int _dy)
	{
		FilterScroll(&_dx, &_dy);
		Scroll(_dx, _dy);
	}

	bool TileLayer::CanScrollHoriz(float dx) const
	{
		return (m_config.viewWindow.x >= -dx) && (m_config.viewWindow.x + m_config.viewWindow.w + dx) <= (m_config.totalCols * m_config.tileWidth);
	}

	bool TileLayer::CanScrollVert(float dy) const
	{
		return (m_config.viewWindow.y >= -dy) && (m_config.viewWindow.y + m_config.viewWindow.h + dy) <= (m_config.totalRows * m_config.tileHeight);
	}

	bool TileLayer::LoadFromCSV(const std::string& context)
	{
		std::istringstream stream(context);
		std::string line;
		Dim row = 0;

		while (std::getline(stream, line) && row < m_config.totalRows)
		{
			if (line.empty())
				continue;

			std::istringstream lineStream(line);
			std::string cell;
			Dim col = 0;

			while (std::getline(lineStream, cell, ',') && col < m_config.totalCols)
			{
				int value = std::stoi(cell);

				if (value == -1)
					SetTile(col, row, MakeIndex(UINT16_MAX, UINT16_MAX));
				else
				{
					// Convert linear index to tileset grid position
					Dim tileRow = value / m_config.tilesetCols;
					Dim tileCol = value % m_config.tilesetCols;
					// Calculate pixel position with offset and margin
					Dim px = m_config.tilesetOffsetX + tileCol * (m_config.tileWidth + m_config.tilesetMarginX);
					Dim py = m_config.tilesetOffsetY + tileRow * (m_config.tileHeight + m_config.tilesetMarginY);
					SetTile(col, row, MakeIndex(py, px));
				}

				++col;
			}

			if (col != m_config.totalCols)
				return false; // Column count mismatch

			++row;
		}

		if (row != m_config.totalRows)
			return false; // Row count mismatch

		return true;
	}

	void TileLayer::SetTileset(Bitmap tileset)
	{
		if (m_tileset)
			BitmapDestroy(m_tileset);
		m_tileset = tileset;
	}
}