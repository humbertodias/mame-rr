const char glsl_bilinear_rgb32_dir_fsh_src[] =
"\n"
"#pragma optimize (on)\n"
"#pragma debug (off)\n"
"\n"
"uniform sampler2D     color_texture;\n"
"uniform vec2          color_texture_pow2_sz; // pow2 tex size\n"
"uniform vec4          vid_attributes;        // gamma, contrast, brightness\n"
"\n"
"// #define DO_GAMMA  1 // 'pow' is very slow on old hardware, i.e. pre R600 and 'slow' in general\n"
"\n"
"#define TEX2D(v) texture2D(color_texture,(v))\n"
"\n"
"void main()\n"
"{\n"
"	vec2 xy = gl_TexCoord[0].st;\n"
"\n"
"	// mix(x,y,a): x*(1-a) + y*a\n"
"	//\n"
"	// bilinear filtering includes 2 mix:\n"
"	//\n"
"	//   pix1 = tex[x0][y0] * ( 1 - u_ratio ) + tex[x1][y0] * u_ratio\n"
"	//   pix2 = tex[x0][y1] * ( 1 - u_ratio ) + tex[x1][y1] * u_ratio\n"
"	//   fin  =    pix1     * ( 1 - v_ratio ) +     pix2    * v_ratio\n"
"	//\n"
"	// so we can use the build in mix function for these 2 computations ;-)\n"
"	//\n"
"	vec2 uv_ratio     = fract(xy*color_texture_pow2_sz); // xy*color_texture_pow2_sz - floor(xy*color_texture_pow2_sz);\n"
"	vec2 one          = 1.0/color_texture_pow2_sz;\n"
"\n"
"#if 1\n"
"	vec4 col, col2;\n"
"\n"
"	col  = mix( TEX2D(xy                   ), TEX2D(xy + vec2(one.x, 0.0)), uv_ratio.x);\n"
"	col2 = mix( TEX2D(xy + vec2(0.0, one.y)), TEX2D(xy + one             ), uv_ratio.x);\n"
"	col  = mix ( col, col2, uv_ratio.y );\n"
"#else\n"
"	// doesn't work on MacOSX GLSL engine ..\n"
"	//\n"
"	vec4 col = mix ( mix( TEX2D(xy                   ), TEX2D(xy + vec2(one.x, 0.0)), uv_ratio.x),\n"
"	                 mix( TEX2D(xy + vec2(0.0, one.y)), TEX2D(xy + one             ), uv_ratio.x), uv_ratio.y );\n"
"#endif\n"
"\n"
"	// gamma, contrast, brightness equation from: rendutil.h / apply_brightness_contrast_gamma_fp\n"
"\n"
"#ifdef DO_GAMMA\n"
"	// gamma/contrast/brightness\n"
"	vec4 gamma = vec4(1.0 / vid_attributes.r, 1.0 / vid_attributes.r, 1.0 / vid_attributes.r, 0.0);\n"
"	gl_FragColor =  ( pow ( col, gamma ) * vid_attributes.g) + vid_attributes.b - 1.0;\n"
"#else\n"
"	// contrast/brightness\n"
"	gl_FragColor =  ( col                * vid_attributes.g) + vid_attributes.b - 1.0;\n"
"#endif\n"
"}\n"
"\n"
;
