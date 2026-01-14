//#pragma once 
//
//#include "Utils/Common.h"
//#include "Rendering/Bitmap.h"
//
//#include <functional>
//
//namespace scene 
//{
//	using namespace gfx;
//
//	struct TileLayerConfig
//	{
//		int	tileWidth = 16;
//		int	tileHeight = 16;
//		int	gridElementWidth = 8;
//		int gridElementHeight = 8;
//	};
//
//	class TileLayer
//	{
//	public:
//		using GridDpyFunk = std::function<void(Bitmap&, int, int, int, int)>;
//
//	public:
//		void SetTile(Dim col, Dim row, Index index);
//		Index GetTile(Dim col, Dim row) const;
//		const Point Pick(Dim x, Dim y) const;
//
//		const Rect& GetViewWindow(void);
//		void SetViewWindow(const Rect& r);
//		void Display(Bitmap& dest, const Rect& displayArea);
//		Bitmap GetBitmap(void) const;
//		GridLayer* GetGrid();
//
//		int GetPixelWidth(void) const;
//		int GetPixelHeight(void) const;
//
//		unsigned GetTileWidth(void) const;
//		unsigned GetTileHeight(void) const;
//
//		void	PutTile(Bitmap& dest, Dim x, Dim y, Bitmap& tiles, Index tile);
//		Dim		TileY(Index index);
//		Dim		TileX(Index index);
//		Index	MakeIndex(byte row, byte col);
//
//		void Scroll(float dx, float dy);
//		void FilterScrollDistance(int viewStartCoord, int viewSize, int* d, int maxMapSize);
//		void FilterScroll(int* dx, int* dy);
//		void ScrollWithBoundsCheck(int _dx, int _dy);
//		bool CanScrollHoriz(float dx) const;
//		bool CanScrollVert(float dy) const;
//
//		auto ToString(void) const -> const std::string; // unparse
//		bool FromString(const std::string&); // parse
//		void Save(const std::string& path) const;
//		bool Load(const std::string& path);
//		FILE* WriteText(FILE* fp) const;
//		bool ReadText(FILE* fp);
//
//		void DisplayGrid(Bitmap& dest, const GridDpyFunk& display_f);
//
//		TileLayer(const TileLayerConfig& config);
//		~TileLayer();
//
//	private:
//		void Allocate();
//
//	private:
//		friend class GridLayer;
//
//	private:
//		TileLayerConfig m_Config;
//
//		int			m_ID = 0;
//		Index*		m_Map = nullptr;
//		GridLayer*	m_grid = nullptr;
//		Dim			m_Totalrows = 0, m_Totalcolumns = 0;
//		Bitmap		m_Tileset = nullptr;
//		Rect		m_ViewWindow{};
//		Bitmap		m_DpyBuffer = nullptr;
//		bool		m_DpyChanged = 0;
//		Dim			m_DpyX = 0, m_DpyY = 0;
//	};
//}