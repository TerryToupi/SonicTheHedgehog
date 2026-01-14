//#pragma once 
//
//#include "Utils/Common.h"
//#include "Rendering/Bitmap.h"
//
//namespace scene 
//{
//	using namespace gfx;
//
//	class TileLayer;
//
//	class GridLayer
//	{
//	public:
//		GridLayer(unsigned rows, unsigned cols);
//		~GridLayer();
//
//		void FilterGridMotion(Rect& r, int* dx, int* dy);
//
//		bool IsOnSolidGround(Rect& r);
//		GridIndex*& GetBuffer(void) { return m_grid; }
//		void SetGridTile(Dim col, Dim row, GridIndex index);
//		GridIndex GetGridTile(Dim col, Dim row);
//		void SetSolidGridTile(Dim col, Dim row);
//		void SetEmptyGridTile(Dim col, Dim row);
//		void SetGridTileFlags(Dim col, Dim row, GridIndex flags);
//		void SetGridTileTopSolidOnly(Dim col, Dim row);
//		bool CanPassGridTile(Dim col, Dim row, GridIndex flags);
//		GridIndex* GetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols);
//		void SetGridTileBlock(Dim colTile, Dim rowTile, Dim tileCols, GridIndex flags);
//
//		void ComputeTileGridBlock(TileLayer* tlayer, Dim row, Dim col, Dim tileCols, byte solidThreshold, bool assumtedEmpty);
//
//	private:
//		void Allocate(void);
//		void FilterGridMotionDown(Rect& r, int* dy);
//		void FilterGridMotionLeft(Rect& r, int* dx);
//		void FilterGridMotionRight(Rect& r, int* dx);
//		void FilterGridMotionUp(Rect& r, int* dx);
//		bool ComputeIsGridIndexEmpty(Bitmap& gridElem, byte solidThreshold);
//		void ComputeGridBlock(GridIndex* grid, Bitmap& tileElem, Bitmap& gridElem, Bitmap& tileSet, byte solidThreshold);
//
//	private:
//		GridIndex* m_grid = nullptr;
//		unsigned m_total = 0;
//		Dim m_totalRows = 0, m_totalColumns = 0;
//	};
//}
