////////////////////////////////////////////////////////////////////////////////
#include "Core.h"
////////////////////////////////////////////////////////////////////////////////
// includes, system
#include "GL_Main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
////////////////////////////////////////////////////////////////////////////////
// includes, GL
////////////////////////////////////////////////////////////////////////////////
#include "DrawUtils.h"
#include "bitmap_fonts.h"
#include "mmsystem.h"
////////////////////////////////////////////////////////////////////////////////
#include "RLE4.h"

Bmp bmp[3];

void set_voxel(uint x,uint y,uint z,uchar4 color)
{ 
	if(x>1023)return;
	if(y>1023)return;
	if(z>1023)return;

	int ofsxy=(x+y*1024)*4;
	int ofsxz=(x+z*1024)*4;
	int ofsyz=(y+z*1024)*4;

	if( z/4 <= bmp[0].data[ofsxy+0] ) { bmp[0].data[ofsxy+0]=z/4; bmp[0].data[ofsxy+1]=color.y; }
	if( y/4 <= bmp[1].data[ofsxz+0] ) { bmp[1].data[ofsxz+0]=y/4; bmp[1].data[ofsxz+1]=color.y; }
	if( x/4 <= bmp[2].data[ofsyz+0] ) { bmp[2].data[ofsyz+0]=x/4; bmp[2].data[ofsyz+1]=color.y; }

	if( z/4 >= bmp[0].data[ofsxy+2] ) { bmp[0].data[ofsxy+2]=z/4; bmp[0].data[ofsxy+3]=color.y; }
	if( y/4 >= bmp[1].data[ofsxz+2] ) { bmp[1].data[ofsxz+2]=y/4; bmp[1].data[ofsxz+3]=color.y; }
	if( x/4 >= bmp[2].data[ofsyz+2] ) { bmp[2].data[ofsyz+2]=x/4; bmp[2].data[ofsyz+3]=color.y; }
}
void init_voxel()
{
	loopi(0,3)
	{
		bmp[i].set(1024,1024,32,0);

		loopj(0,1024*1024)
		{
			bmp[i].data[j*4+0]=255;
			bmp[i].data[j*4+1]=0;
			bmp[i].data[j*4+2]=0;
			bmp[i].data[j*4+3]=0;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
// rendering callbacks
void display();
///////////////////////////////////////////
static glShaderManager shader_manager;
static GLuint tex_screen=-2;
vec3f forward1;
vec3f side1;
vec3f updown1;
///////////////////////////////////////////
// Program main
///////////////////////////////////////////
const int		bean_add_tex_res=1024;
float			bean_add_tex[bean_add_tex_res*4];
uchar			bean_add_tex8[bean_add_tex_res*4];
///////////////////////////////////////////
void create_bean_add_tex()
{
	//Bmp circ(1024,1024,24,0);
	//memset(circ.data,0,1024*1024*3);

	for(int i=0;i<bean_add_tex_res;i++)
	{
		float a=float(i)/float(bean_add_tex_res);
		float r=a*2*M_PI;
		float x=sin(r*3)*0.125+cos(r*5)*0.125+0.25;
		float y=cos(r*3)*0.125+sin(r*5)*0.125+0.25;
		bean_add_tex[i*4+0]=x;
		bean_add_tex[i*4+1]=y;
		bean_add_tex[i*4+2]=r*2;
		bean_add_tex[i*4+3]=sqrt(a);
		bean_add_tex8[i*4+0]=255*x;
		bean_add_tex8[i*4+1]=255*y;
		bean_add_tex8[i*4+2]=255*r*2;
		float a05=float((i+bean_add_tex_res/2)%bean_add_tex_res)/float(bean_add_tex_res);
		bean_add_tex8[i*4+3]=255*sqrt(a05);
	}
}
///////////////////////////////////////////
const int		bean_rad_tex_res=1024;
float			bean_rad_tex[bean_rad_tex_res];
///////////////////////////////////////////
void create_bean_circle_tex()
{
	//Bmp circ(1024,1024,24,0);
	//memset(circ.data,0,1024*1024*3);
	for(int i=0;i<bean_rad_tex_res;i++)bean_rad_tex[i]=0;

	for (float b=0;b<2*M_PI;b+=2*M_PI/3)
	for (float a=0;a<2*M_PI;a+=1.0/(1*float(bean_rad_tex_res)))
	{
		float x=sin(b)*200+sin(a)*280;
		float y=cos(b)*200+cos(a)*280;
		float r = sqrt(x*x+y*y);
		float alpha=(atan2(x,y)/M_PI+1)*float(bean_rad_tex_res)*0.5;
		int index=int(alpha);
		if(index>=0 && index<bean_rad_tex_res)
		{
			if(r>bean_rad_tex[index])
				bean_rad_tex[index]=r;
		}
		//circ.set_pixel(x+512,y+512,55,55,55);
	}
	/*
	for(int i=0;i<bean_rad_tex_res;i++)
	{
		float a=2*M_PI*float(i)/float(bean_rad_tex_res);
		float r=bean_rad_tex[i]+20;
		float x=sin(a)*r;
		float y=-cos(a)*r;
		circ.set_pixel(x+512,y+512,255,255,255);
	}
	*/
	for(int i=0;i<bean_rad_tex_res;i++)bean_rad_tex[i]/=480.0*4.0;
	//circ.save("test_circ.bmp");
}
///////////////////////////////////////////
const int		bean_branch_tex_res=512;
float			bean_branch_tex[bean_branch_tex_res*bean_branch_tex_res*4];
uchar			bean_branch_tex8[bean_branch_tex_res*bean_branch_tex_res*4];
///////////////////////////////////////////
void create_bean_branch_tex()
{
	Bmp test(bean_branch_tex_res,bean_branch_tex_res,24,0);
	memset(test.data,0,bean_branch_tex_res*bean_branch_tex_res*3);

	for(int i=0;i<bean_branch_tex_res*bean_branch_tex_res*4;i+=4)
	{
		bean_branch_tex[i+0]=0;
		bean_branch_tex[i+1]=1;
		bean_branch_tex[i+2]=0;
		bean_branch_tex[i+3]=0;
		bean_branch_tex8[i+0]=0;
		bean_branch_tex8[i+1]=255;
		bean_branch_tex8[i+2]=0;
		bean_branch_tex8[i+3]=0;
	}

	for (int y=0;y<bean_branch_tex_res;y+=1)
	for (int b=0;b<10;b+=1)
	for (float a=0;a<2*M_PI;a+=1.0/(2*float(100)))
	{
		int rnd1=((b*35247+3373799)^48139)&15;
		int rnd2=((b*64235+7226163)^73513)&15;
		int rnd3=((b*73357+5342356)^23665)&15;
		float yr=2*M_PI*float(y)/float(bean_branch_tex_res);
		float cosa=cos(a);

		float radius=float((y+rnd1*bean_branch_tex_res/15)&(bean_branch_tex_res-1))/float(bean_branch_tex_res);

		float x=2*sin(yr*2+2+rnd2)*5+3*sin(yr-rnd3)*5+sqrt(radius)*sin(a)*40+b*500/10+512;
		float z=3*cos(yr*2+2+rnd2)*5+2*cos(yr-rnd3)*5+sqrt(radius)*cosa*40+80-radius*120;
		if(z<0)z=0;
		if(z>511)z=511;
		x=int(x)&(bean_branch_tex_res-1);
		//z=int(z)&511;

		int ofs=int(x)+y*bean_branch_tex_res;

		if(cosa>0)
		{
			test.set_pixel(x,z,255,0,0);
			float zd=z/200;
			if(zd>bean_branch_tex[ofs*4+0])
			{
				bean_branch_tex[ofs*4+0]=zd;
				bean_branch_tex8[ofs*4+0]=zd*255;
			}
		}
		else
		{
			float zd=z/200;
			//if(zd>bean_branch_tex[ofs*4+1])
			{
				bean_branch_tex[ofs*4+1]=zd;
				bean_branch_tex8[ofs*4+1]=zd*255;
			}
		//	test.set_pixel(x,z,0,255,0);
			//bean_branch_tex[ofs*4+1]=z/200.0;
			//bean_branch_tex8[ofs*4+1]=z*255/200.0;
		}
	//	test.set_pixel(x,20,0,0,255);

		int     groundx= sin(a)*40 + 2*sin(yr*3+1+rnd1*3)*10 + 3*cos(yr-rnd2*4-4)*10+b*500/10;
		float groundz= cos(a)*20; 
		ofs=groundx+y*bean_branch_tex_res;
		groundz/=200;
		groundx%=bean_branch_tex_res;
		if(groundz>0)
		if(groundz>bean_branch_tex[ofs*4+2])
		{
			bean_branch_tex[ofs*4+2]=groundz;
			bean_branch_tex8[ofs*4+2]=groundz*255;
		}
	}
	for(int i=0;i<bean_branch_tex_res*bean_branch_tex_res*4;i++)
	{
		bean_branch_tex[i]=1.0-bean_branch_tex[i];
		bean_branch_tex8[i]=255-bean_branch_tex8[i];
	}

	//test.save("test_test.bmp");
}
///////////////////////////////////////////
int main( int argc, char** argv) {

	init_voxel();
	RLE4 rle;
	//rle.load_m5("../voxelstein.rle4");//spheres2.rle4");
	//rle.load_m5("../dragon.rle4");
	//rle.load_m5("../sphereN.rle4");
	rle.load_m5("../spheres2.rle4");
	//rle.load_m5("../voxelstein.rle4");
	

	printf("create_bean_circle_tex()\n");
	create_bean_circle_tex();
	printf("create_bean_branch_tex()\n");
	create_bean_branch_tex();
	printf("create_bean_add_tex()\n");
	create_bean_add_tex();
	

	timeBeginPeriod(1); 

	screen.posx = -1.12;
	screen.posy =  0;//-0.46+15;
	screen.posz =  0.18;

	gl_main.Init(SCREEN_SIZE_X,SCREEN_SIZE_Y,false,display);
	wglSwapIntervalEXT(0);

    glutMainLoop();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
void update_viewpoint()
{
	//int a;loop ( a,0,128 ) if ( keyboard.KeyDn(a) ) printf("\nk:%d\n",a);

	static float accel = 0;

	static int time1=0,time2=timeGetTime(),delta=0; 
	time1 = time2; time2 = timeGetTime(); delta=time2-time1;

	static float multiplier = 0.25f;
	float step = -0.01;//float(delta) * multiplier;

	screen.rotx = -(mouse.mouseY-0.5)*10+10.01;
	screen.roty =  (mouse.mouseX-0.5)*10+10.01+M_PI/2;

	//if(screen.rotx> M_PI-0.01)screen.rotx= M_PI-0.01;
	//if(screen.rotx<-M_PI+0.01)screen.rotx=-M_PI+0.01;

	////////////////////// Direction matrix
	matrix44 m;
	m.ident();
	m.rotate_z(-screen.rotz );
	m.rotate_x(screen.rotx );
	m.rotate_y(screen.roty );

	forward1  = m * vec3f(0,0,-1).v3();
	side1	       = m * vec3f(1,0,0).v3();
	updown1  = m * vec3f(0,-1,0).v3();

	////////////////////// Transform direction vector
	vec3f pos( screen.posx,screen.posy,screen.posz );
	vec3f forward  = m * vec3f(0,0,-step).v3();
	vec3f side	       = m * vec3f(-step,0,0).v3();
	vec3f updown  = m * vec3f(0,step,0).v3();

	vec3f pos_before=pos;
	int  f_ground = 2048;

	//printf("\n");
	//for ( int k=0;k<128;k++) if ( keyboard.KeyDn(k)) printf("\nKeyDn:%d ",k);

	vec3f posgr = pos_before; posgr.y-=1;

	if ( keyboard.KeyDn(119) ) pos = pos - forward;
	if ( keyboard.KeyDn(115) ) pos = pos + forward;
	if ( keyboard.KeyDn(97 ) ) pos = pos - side;
	if ( keyboard.KeyDn(100) ) pos = pos + side;
	if ( keyboard.KeyDn(113) ) pos = pos + updown;
	if ( keyboard.KeyDn(101) ) pos = pos - updown;
	if ( keyboard.KeyPr(102) ) gl_main.ToggleFullscreen();
	if ( keyboard.KeyPr(43 ) ) multiplier *=2;
	if ( keyboard.KeyPr(45 ) ) if(multiplier>0.01) multiplier /=2;
	if ( keyboard.KeyPr(' ' )) accel =3;

	screen.posx = pos.x;
	screen.posy = pos.y;
	screen.posz = pos.z;
}
////////////////////////////////////////////////////////////////////////////////
//! GLUT Display callback
////////////////////////////////////////////////////////////////////////////////
bool is_inside ( 
				float cx,float cy,float cz, 
				float sx,float sy,float sz, 
				float ex,float ey,float ez )
{
	float tolerance = 0.13;
	if ( cx >= sx-tolerance )
	if ( cy >= sy-tolerance )
	if ( cz >= sz-tolerance )
	if ( cx <= ex+tolerance )
	if ( cy <= ey+tolerance )
	if ( cz <= ez+tolerance ) return true;
	return false;
}

// this method is used to draw the front and backside of the volume
void drawQuads(float sx, float sy, float sz,float addx,float addy,float addz,glShader* shade,matrix44& m)
{	
	matrix44 mi = m;
	mi.invert_simpler();
	
	shade->setUniform3f("xform0",mi.m[0][0]/sx ,mi.m[1][0]/sx,mi.m[2][0]/sx);	
	shade->setUniform3f("xform1",mi.m[0][1]/sy ,mi.m[1][1]/sy,mi.m[2][1]/sy);	
	shade->setUniform3f("xform2",mi.m[0][2]/sz ,mi.m[1][2]/sz,mi.m[2][2]/sz);	
	
	vector4 rel_pos=mi*vector4 (
		screen.posx-addx,
		screen.posy-addy,
		screen.posz-addz,0);

	float x_sign=1;
	
	if(fabs(rel_pos.x)<0.5*sx+0.1 &&
	   fabs(rel_pos.y)<0.5*sy+0.1 &&
	   fabs(rel_pos.z)<0.5*sz+0.1)
	{
		shade->setUniform1f("insideCube",1);	
		x_sign=-1;
	}
	else
	{
		shade->setUniform1f("insideCube",-1);	
		x_sign=1;
	}
	
	vector4 rel_pos_cube=mi*rel_pos;

	glBegin(GL_QUADS);

	for (int sides=0;sides<6;sides++)
	{
		float xa[4]={-1,1,1,-1};
		float ya[4]={-1,-1,1,1};
		float za[4]={ 1, 1, 1, 1};

		for (int i=0;i<4;i++)
		{
			float p[3];
			if (sides==0){ 	p[0]= xa[i];p[1]=ya[i];p[2]=za[i]; }			
			if (sides==1){ 	p[0]=-xa[i];p[1]=ya[i];p[2]=-za[i]; }			
			if (sides==2){ 	p[0]=-xa[i];p[2]=ya[i];p[1]= za[i]; }			
			if (sides==3){ 	p[0]= xa[i];p[2]=ya[i];p[1]=-za[i]; }			
			if (sides==4){ 	p[2]=-xa[i];p[1]=ya[i];p[0]= za[i]; }			
			if (sides==5){ 	p[2]= xa[i];p[1]=ya[i];p[0]=-za[i]; }			

			float x=p[0]*0.5*x_sign;
			float y=p[1]*0.5;
			float z=p[2]*0.5;
			vector4 xf=m*vector4(x*sx,y*sy,z*sz,0);

			glMultiTexCoord3fARB(GL_TEXTURE0_ARB, x, y, z);
			glVertex3f(
				xf.x+addx,
				xf.y+addy,
				xf.z+addz);
		}	
	}
	glEnd();

	return;
}

void
display() 
{
//	static int tex_bean_rad=gl_main.createFloatTexture( bean_rad_tex_res,1,(char*) bean_rad_tex,1) ;
	//static int tex_bean_branch=gl_main.createFloatTexture( bean_branch_tex_res,bean_branch_tex_res,(char*) bean_branch_tex,4,true) ;
//	static int tex_bean_branch8=gl_main.createFloatTexture( bean_branch_tex_res,bean_branch_tex_res,(char*) bean_branch_tex8,4,false) ;
//	static int tex_bean_add=gl_main.createFloatTexture(bean_add_tex_res,1,(char*) bean_add_tex8,4,false);

	static int tex_z=gl_main.createTexture(bmp[0].width,bmp[0].height,32,bmp[0].data);
	static int tex_y=gl_main.createTexture(bmp[1].width,bmp[1].height,32,bmp[1].data);
	static int tex_x=gl_main.createTexture(bmp[2].width,bmp[2].height,32,bmp[2].data);

	update_viewpoint();

	glClearColor(0.5, 0.5, 0.5, 0);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable (GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(-screen.rotx*180/M_PI,1,0,0);
	glRotatef(-screen.roty*180/M_PI,0,1,0);
	glTranslatef(-screen.posx,-screen.posy,-screen.posz);

	static glShader* shader_soft=shader_soft = shader_manager.loadfromFile(
		"../shader/soft.vert",
		"../shader/soft.frag");

	//static int tex_bean = gl_main.LoadTex("../branches_detail2.bmp");

	glActiveTextureARB( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, tex_z);
	glActiveTextureARB( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, tex_y);
	glActiveTextureARB( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, tex_x);

	glEnable(GL_CULL_FACE);
	glCullFace (GL_BACK);
	
	shader_soft->begin();
	
	shader_soft->setUniform1i("tex_z",0);	
	shader_soft->setUniform1i("tex_y",1);	
	shader_soft->setUniform1i("tex_x",2);	
	shader_soft->setUniform3f("viewpoint",screen.posx,screen.posy,screen.posz);	
	shader_soft->setUniform3f("xform0",1,0,0);	
	shader_soft->setUniform3f("xform1",0,1,0);	
	shader_soft->setUniform3f("xform2",0,0,1);	
	
	//glDisable (GL_DEPTH_TEST);
		
	matrix44 m;
	m.ident();
	m.rotate_x(M_PI/2);
	for(int a=5;a>0;a--)
	for(int b=5;b>0;b--)
	{
		//m.rotate_z(-screen.rotz );
		//m.rotate_x(a );
		//m.rotate_y(b );

		drawQuads(2,2,2,a*2,0,b*2,shader_soft,m);
	}

	shader_soft->end();

	glActiveTextureARB( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, 0);
	glActiveTextureARB( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, 0);
	glActiveTextureARB( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, 0);

	static float msavg=1.0;
	static int t_1=timeGetTime();
	static int t_2;
	t_2=t_1; t_1=timeGetTime();

	msavg = ( msavg * 3 + float(t_1-t_2) ) / 4.0;

	beginRenderText( screen.window_width, screen.window_height);
    {
		char text[100];
		int HCOL= 0,VCOL = 5;
        glColor3f( 1.0f, 1.0f, 1.0f );
		sprintf(text, "Total: %3.2f msec (%3.3f fps)", msavg, (1000.0f/msavg) );
		HCOL+=15;
        renderText( VCOL, HCOL, BITMAP_FONT_TYPE_HELVETICA_12, text );
		sprintf(text, "Pos %2.2f %2.2f %2.2f", screen.posx,screen.posy,screen.posz);
		HCOL+=15;
        renderText( VCOL, HCOL, BITMAP_FONT_TYPE_HELVETICA_12, text );
		sprintf(text, "Rot %2.2f %2.2f %2.2f", screen.rotx,screen.roty,screen.rotz);
		HCOL+=15;
        renderText( VCOL, HCOL, BITMAP_FONT_TYPE_HELVETICA_12, text );
    }
    endRenderText();
	
	glutSwapBuffers();
}
////////////////////////////////////////////////////////////////////////////////
