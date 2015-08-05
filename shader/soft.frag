uniform sampler2D tex_x;
uniform sampler2D tex_y;
uniform sampler2D tex_z;
varying vec3 texCoord;
varying vec3 vertex;
varying vec4 vertex_screen;
uniform vec3 viewpoint;
uniform float insideCube;

uniform vec3 xform0;
uniform vec3 xform1;
uniform vec3 xform2;

void main(void)
{
	//if(vertex_screen.z/100>gl_FragCoord.z) discard;

	vec3 delta_raw=vertex-viewpoint;
	vec3 delta;
	delta.x= dot(xform0,delta_raw);
	delta.y= dot(xform1,delta_raw);
	delta.z= dot(xform2,delta_raw);

	vec3 dir_in = normalize( delta ) ;
	vec3 pos_in = texCoord;

	float ray_len=0.004;
	float accum_len=0;

	if(insideCube>0) 
	{
		pos_in -= delta;
		dir_in*= ray_len;
	}
	else
	{
		ray_len+=vertex_screen.z*0.001535;
		dir_in*= ray_len;
	}

	vec3 dir = dir_in.xzy;
	vec3 pos = pos_in.xzy;

	vec3 dir_sign = vec3(1,1,1);
	if ( dir.x < 0 ) dir_sign.x=-1;
	if ( dir.y < 0 ) dir_sign.y=-1;
	if ( dir.z < 0 ) dir_sign.z=-1;

   	vec4  col;

	float accum=1;

	vec3 tpos=pos+vec3(0.5,0.5,0.5);

	while (1)
	{	

		vec4 tx=texture2D(tex_x,tpos.xy); 
		if( tpos.z >= tx.x ) 
		if( tpos.z <= tx.z ) 
		{
			vec4 ty=texture2D(tex_y,tpos.xz); 
			if( tpos.y >= ty.x ) 
			if( tpos.y <= ty.z ) 
			{
				vec4 tz=texture2D(tex_z,tpos.yz); 
				if( tpos.x >= tz.x ) 
				if( tpos.x <= tz.z ) 
				{
					gl_FragColor.x=tz.y;//tpos.x;
					gl_FragColor.y=tz.y;//tpos.y;
					gl_FragColor.z=tz.y;//tpos.z;
					gl_FragColor.w=1.0;
					//gl_FragDepth = vertex_screen.z*0.01;//+normalize(vertex_screen.xyz).z * accum_len*0.001;
					return;
				}
			}
		}

		vec3 cdir=dir*accum;
		pos += cdir;
		tpos+= cdir;
		accum_len+=accum;
		accum+=0.004;

		if (pos.x * dir_sign.x -0.5 > 0 || 
			pos.y * dir_sign.y -0.5 > 0 || 
			pos.z * dir_sign.z -0.5 > 0 ) break;
	}
	//gl_FragColor.xyz = texCoord;
	//gl_FragDepth=1;

	//gl_FragColor.xyz = gl_FragCoord.zzz*0.0;    
//	gl_FragDepth=vertex_screen.z/100;//+normalize(vertex_screen.xyz).z * accum_len;
	discard;

}