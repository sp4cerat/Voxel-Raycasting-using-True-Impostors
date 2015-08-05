#pragma once
#include "Core.h"
#include <vector>

struct Map4
{
	int    sx,sy,sz,slabs_size;
	uint   *map_mem;
	uint   *map;
	ushort *slabs;
	//uint	nop;

	// map:
	// id 0 : rle siz
	// id 0 : tex siz
	// id 1 : rle len
	// id 2 : rle elems ... []
	// id x : tex elems ... []
};

struct Map5
{
	int    sx,sy,sz; // total size
	int    tx,ty,tz; // tile size

	struct Tile
	{
		uint   slabs_size;
		uint   *map;
		ushort *slabs;
	};

	Tile tiles[16*16];
};


class Tree;

struct RLE4
{	
	/*------------------------------------------------------*/
	Map4 map [16],mapgpu [16];int nummaps;
	Map5 map5[16],mapgpu5[16];
	/*------------------------------------------------------*/
	void compress_all(Tree& tree);
	/*------------------------------------------------------*/
	Map4 compress(Tree& tree,int mip_lvl); //int sx,int sy,int sz,uchar* col1,uchar* col2);
	/*------------------------------------------------------*/
	void init();
	/*------------------------------------------------------*/
	void clear();
	/*------------------------------------------------------*/
	void save(char *filename);
	/*------------------------------------------------------*/
	bool load(char *filename);
	/*------------------------------------------------------*/
	bool load_m5(char *filename);
	/*------------------------------------------------------*/
	void setgeom (long x, long y, long z, long issolid);
	void setcol (long x, long y, long z, long argb);
	long loadvxl (char *filnam);
	long loadvxl_octree (char *filnam);
	Map4 compressvxl(ushort* mem,int sx,int sy,int sz,int mip_lvl);
	/*------------------------------------------------------*/
};
