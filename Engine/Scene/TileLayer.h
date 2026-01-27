#pragma once

#include "Rendering/Bitmap.h"
#include "Utils/Common.h"
#include "Core/LatelyDestroyable.h"

#include <vector>
#include <functional>

namespace scene
{
	using namespace gfx;
	using namespace core;

	struct TileConfig
	{
		int			id = 0;
		Rect		viewWindow{};

		Dim	totalCols = 0;
		Dim	totalRows = 0;
		Dim	tileWidth = 16;
		Dim	tileHeight = 16;

		// Tileset layout parameters
		Dim	tilesetCols = 1;       // Number of columns in tileset grid
		Dim	tilesetOffsetX = 0;    // Pixel offset from left edge
		Dim	tilesetOffsetY = 0;    // Pixel offset from top edge
		Dim	tilesetMarginX = 0;    // Horizontal margin between tiles
		Dim	tilesetMarginY = 0;    // Vertical margin between tiles
	};

	class TileLayer : public LatelyDestroyable
	{
	public:
		void				Configure(TileConfig& cfg);
		const TileConfig&	Config(void);

		void		SetTile(Dim col, Dim row, Index index);
		Index		GetTile(Dim col, Dim row) const;
		const Point Pick(Dim x, Dim y);

		void		PutTile(Bitmap& dest, Dim x, Dim y, Bitmap& tiles, Index tile);
		static Dim			TileY(Index index);
		static Dim			TileX(Index index);
		static Index		MakeIndex(Dim row, Dim col);

		const Rect& GetViewWindow(void);
		void		SetViewWindow(const Rect& r);
		void		Display(Bitmap& dest, const Point& dp);
		Bitmap		GetBitmap(void) const;
		int			GetPixelWidth(void) const;
		int			GetPixelHeight(void) const;
		unsigned	GetTileWidth(void);
		unsigned	GetTileHeight(void);

		void Scroll(float dx, float dy);
		void FilterScrollDistance(int viewStartCoord, int viewSize, int* d, int maxMapSize);
		void FilterScroll(int* dx, int* dy);
		void ScrollWithBoundsCheck(int _dx, int _dy);
		bool CanScrollHoriz(float dx) const;
		bool CanScrollVert(float dy) const;

		bool LoadFromCSV(const std::string& context);
		void SetTileset(Bitmap tileset);

		TileLayer() = default;
		~TileLayer();

	private:
		inline Index DivTileWidth(Index i);
		inline Index DivTileHeight(Index i);
		inline Index MulTileHeight(Index i);
		inline Index MulTileWidth(Index i);
		inline Index ModTileWidth(Index i);
		inline Index ModTileHeight(Index i);

	private:
		using MapContainer = std::vector<Index>;

		TileConfig	 m_config{};

		MapContainer m_map;
		Bitmap		 m_tileset = nullptr;
		Bitmap		 m_dpyBuffer = nullptr;
		bool		 m_dpyChanged = true;
		Dim			 m_dpyX = 0, m_dpyY = 0;
	};
}