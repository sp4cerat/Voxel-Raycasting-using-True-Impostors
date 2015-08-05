///////////////////////////////////////////
#pragma once
///////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "VecMath.h"
///////////////////////////////////////////
class Bmp
{
public:

	Bmp();
	Bmp(int x,int y,int bpp,unsigned char*data);
	Bmp(const char*filename);
	~Bmp();

	bool load(const char*filename,
		bool checktransparency=false,
		int check_r=0,
		int check_g=0, 
		int check_b=0);

	bool save(const char*filename);

	vec3f getSxSyT(float x);
	int  sampleByte(int x,int y);
	bool set(int x,int y,int bpp,unsigned char*data);
	bool set3d(int x,int y,int z,int bpp,unsigned char*buffer);
	void crop(int x,int y);
	bool scale(int x,int y);
	bool blur(int count);
	bool hblur(int count);
	bool vblur(int count);
	bool addalpha(unsigned char r,unsigned char g,unsigned char b);
	bool normalize(void);
	bool normalMap(void);
	vec3f getPixel(float x,float y);
	vec3f get_f_fdx_fdy(float x,float y);
	void set_pixel(int x,int y,int r,int g,int b)
	{
		if(x>=0 && x<width && y>=0 && y<height)
		{
			int ofs=int(x) + int(y)*width;
			data[ ofs*3+0 ]=b;
			data[ ofs*3+1 ]=g;
			data[ ofs*3+2 ]=r;	
		}
	}

private:

	int  sampleMap(int i,int j);

public:

	unsigned char*data;
	int width;
	int height;
	int depth;
	int bpp;

private:

	unsigned char bmp[54];
};

