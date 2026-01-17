#include "Scene/TileLayer.h"

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

	const TileConfig& TileLayer::Config(void)
	{
		return m_config;
	}

	void TileLayer::SetTile(Dim col, Dim row, Index index)
	{
		m_map[row * m_config.totalRows + col] = index;
	}

	Index TileLayer::GetTile(Dim col, Dim row) const
	{
		return m_map[row * m_config.totalRows + col];
	}

	const Point TileLayer::Pick(Dim x, Dim y) 
	{
		Index ix = DivTileWidth(x + m_config.ViewWindow.x);
		Index iy = DivTileWidth(y + m_config.ViewWindow.y);
		return { (int)ix, (int)iy };
	}

	void TileLayer::PutTile(Bitmap& dest, Dim x, Dim y, Bitmap& tiles, Index tile)
	{
		BitmapBlit(
			m_tileset,
			{ TileX(tile), TileY(tile), m_config.tileWidth, m_config.tileHeight },
			dest,
			{ x, y }
		);
	}

	Dim TileLayer::TileY(Index index)
	{
		return Dim(index >> 16);
	}

	Dim TileLayer::TileX(Index index)
	{
		return Dim(index & 0xFFFF);
	}

	Index TileLayer::MakeIndex(Dim row, Dim col)
	{
		return (Index(row) << 16) | Index(col);
	}
}