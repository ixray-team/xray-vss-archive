#include "stdafx.h"
#include "IDirect3DDevice9.h"

#include "xrD3D9-Null_OutProc.h"

HRESULT		xrIDirect3DDevice9::GetDeviceCaps( D3DCAPS9* pCaps)
{ 
	APIDEBUG("xrIDirect3DDevice9::GetDeviceCaps");

	if (!pCaps)
		return D3DERR_INVALIDCALL;

	memset(pCaps, 0, sizeof(D3DCAPS9));

	/* Device Info */
	pCaps->DeviceType = D3DDEVTYPE_HAL;
	pCaps->AdapterOrdinal = 0;

	/* Caps from DX7 Draw */
	pCaps->Caps = 0; //D3DCAPS_READ_SCANLINE;
	pCaps->Caps2 = D3DCAPS2_CANAUTOGENMIPMAP | D3DCAPS2_CANCALIBRATEGAMMA |
		D3DCAPS2_CANMANAGERESOURCE | D3DCAPS2_DYNAMICTEXTURES |
		D3DCAPS2_FULLSCREENGAMMA;//D3DCAPS2_CANRENDERWINDOWED ;
	pCaps->Caps3 = D3DCAPS3_ALPHA_FULLSCREEN_FLIP_OR_DISCARD | D3DCAPS3_COPY_TO_VIDMEM |
		D3DCAPS3_COPY_TO_SYSTEMMEM | D3DCAPS3_LINEAR_TO_SRGB_PRESENTATION;

	pCaps->PresentationIntervals = D3DPRESENT_INTERVAL_IMMEDIATE; // see wglSwapIntervalEXT

	/* Cursor Caps */
	pCaps->CursorCaps = D3DCURSORCAPS_COLOR;

	pCaps->DevCaps = 
		D3DDEVCAPS_EXECUTESYSTEMMEMORY			
		| D3DDEVCAPS_EXECUTEVIDEOMEMORY			
		| D3DDEVCAPS_TLVERTEXSYSTEMMEMORY		
		| D3DDEVCAPS_TLVERTEXVIDEOMEMORY		
		| D3DDEVCAPS_TEXTURESYSTEMMEMORY		
		| D3DDEVCAPS_TEXTUREVIDEOMEMORY			
		| D3DDEVCAPS_DRAWPRIMTLVERTEX			
		| D3DDEVCAPS_CANRENDERAFTERFLIP			
		| D3DDEVCAPS_TEXTURENONLOCALVIDMEM		
		| D3DDEVCAPS_DRAWPRIMITIVES2			
		| D3DDEVCAPS_SEPARATETEXTUREMEMORIES	
		| D3DDEVCAPS_DRAWPRIMITIVES2EX			
		| D3DDEVCAPS_HWTRANSFORMANDLIGHT		
		| D3DDEVCAPS_CANBLTSYSTONONLOCAL		
		| D3DDEVCAPS_HWRASTERIZATION			
		| D3DDEVCAPS_PUREDEVICE					
		| D3DDEVCAPS_QUINTICRTPATCHES			
		| D3DDEVCAPS_RTPATCHES					
		| D3DDEVCAPS_RTPATCHHANDLEZERO			
		| D3DDEVCAPS_NPATCHES	;				

	pCaps->PrimitiveMiscCaps = D3DPMISCCAPS_MASKZ
		|  D3DPMISCCAPS_CULLNONE					
		|  D3DPMISCCAPS_CULLCW						
		|  D3DPMISCCAPS_CULLCCW						
		|  D3DPMISCCAPS_COLORWRITEENABLE			
		|  D3DPMISCCAPS_CLIPPLANESCALEDPOINTS		
		|  D3DPMISCCAPS_CLIPTLVERTS					
		|  D3DPMISCCAPS_TSSARGTEMP					
		|  D3DPMISCCAPS_BLENDOP						
		|  D3DPMISCCAPS_NULLREFERENCE				
		|  D3DPMISCCAPS_INDEPENDENTWRITEMASKS		
		|  D3DPMISCCAPS_PERSTAGECONSTANT			
		|  D3DPMISCCAPS_FOGANDSPECULARALPHA			
		|  D3DPMISCCAPS_SEPARATEALPHABLEND         
		|  D3DPMISCCAPS_MRTINDEPENDENTBITDEPTHS    
		|  D3DPMISCCAPS_MRTPOSTPIXELSHADERBLENDING 
		|  D3DPMISCCAPS_FOGVERTEXCLAMPED;

	pCaps->RasterCaps =	D3DPRASTERCAPS_DITHER
		|  D3DPRASTERCAPS_ZTEST                  
		|  D3DPRASTERCAPS_FOGVERTEX              
		|  D3DPRASTERCAPS_FOGTABLE               
		|  D3DPRASTERCAPS_MIPMAPLODBIAS          
		|  D3DPRASTERCAPS_ZBUFFERLESSHSR         
		|  D3DPRASTERCAPS_FOGRANGE               
		|  D3DPRASTERCAPS_ANISOTROPY             
		|  D3DPRASTERCAPS_WBUFFER                
		|  D3DPRASTERCAPS_WFOG                   
		|  D3DPRASTERCAPS_ZFOG                   
		|  D3DPRASTERCAPS_COLORPERSPECTIVE       
		|  D3DPRASTERCAPS_SCISSORTEST            
		|  D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS    
		|  D3DPRASTERCAPS_DEPTHBIAS              
		|  D3DPRASTERCAPS_MULTISAMPLE_TOGGLE;

	pCaps->ZCmpCaps = D3DPCMPCAPS_NEVER
		| D3DPCMPCAPS_LESS          
		| D3DPCMPCAPS_EQUAL         
		| D3DPCMPCAPS_LESSEQUAL     
		| D3DPCMPCAPS_GREATER       
		| D3DPCMPCAPS_NOTEQUAL      
		| D3DPCMPCAPS_GREATEREQUAL  
		| D3DPCMPCAPS_ALWAYS;

	pCaps->SrcBlendCaps = D3DPBLENDCAPS_ZERO              
		| D3DPBLENDCAPS_ONE               
		| D3DPBLENDCAPS_SRCCOLOR          
		| D3DPBLENDCAPS_INVSRCCOLOR       
		| D3DPBLENDCAPS_SRCALPHA          
		| D3DPBLENDCAPS_INVSRCALPHA       
		| D3DPBLENDCAPS_DESTALPHA         
		| D3DPBLENDCAPS_INVDESTALPHA      
		| D3DPBLENDCAPS_DESTCOLOR         
		| D3DPBLENDCAPS_INVDESTCOLOR      
		| D3DPBLENDCAPS_SRCALPHASAT       
		| D3DPBLENDCAPS_BOTHSRCALPHA      
		| D3DPBLENDCAPS_BOTHINVSRCALPHA   
		| D3DPBLENDCAPS_BLENDFACTOR ;      

	pCaps->DestBlendCaps = pCaps->SrcBlendCaps;

	pCaps->AlphaCmpCaps = pCaps->ZCmpCaps;

	pCaps->ShadeCaps = D3DPSHADECAPS_COLORGOURAUDRGB       
		|  D3DPSHADECAPS_SPECULARGOURAUDRGB
		|  D3DPSHADECAPS_ALPHAGOURAUDBLEND 
		|  D3DPSHADECAPS_FOGGOURAUD        ;

	pCaps->TextureCaps = D3DPTEXTURECAPS_PERSPECTIVE         
		|  D3DPTEXTURECAPS_POW2                
		|  D3DPTEXTURECAPS_ALPHA               
		|  D3DPTEXTURECAPS_SQUAREONLY          
		|  D3DPTEXTURECAPS_TEXREPEATNOTSCALEDBYSIZE
		|  D3DPTEXTURECAPS_ALPHAPALETTE        
		|  D3DPTEXTURECAPS_NONPOW2CONDITIONAL  
		|  D3DPTEXTURECAPS_PROJECTED           
		|  D3DPTEXTURECAPS_CUBEMAP             
		|  D3DPTEXTURECAPS_VOLUMEMAP           
		|  D3DPTEXTURECAPS_MIPMAP              
		|  D3DPTEXTURECAPS_MIPVOLUMEMAP        
		|  D3DPTEXTURECAPS_MIPCUBEMAP          
		|  D3DPTEXTURECAPS_CUBEMAP_POW2        
		|  D3DPTEXTURECAPS_VOLUMEMAP_POW2      
		|  D3DPTEXTURECAPS_NOPROJECTEDBUMPENV  ;

	pCaps->TextureFilterCaps =  D3DPTFILTERCAPS_MINFPOINT           
		|  D3DPTFILTERCAPS_MINFLINEAR          
		|  D3DPTFILTERCAPS_MINFANISOTROPIC     
		|  D3DPTFILTERCAPS_MINFPYRAMIDALQUAD   
		|  D3DPTFILTERCAPS_MINFGAUSSIANQUAD    
		|  D3DPTFILTERCAPS_MIPFPOINT           
		|  D3DPTFILTERCAPS_MIPFLINEAR          
		|  D3DPTFILTERCAPS_MAGFPOINT           
		|  D3DPTFILTERCAPS_MAGFLINEAR          
		|  D3DPTFILTERCAPS_MAGFANISOTROPIC     
		|  D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD   
		|  D3DPTFILTERCAPS_MAGFGAUSSIANQUAD    ;

	pCaps->CubeTextureFilterCaps = pCaps->TextureFilterCaps;

	pCaps->VolumeTextureFilterCaps = pCaps->TextureFilterCaps;

	pCaps->TextureAddressCaps = D3DPTADDRESSCAPS_WRAP         
		| D3DPTADDRESSCAPS_MIRROR       
		| D3DPTADDRESSCAPS_CLAMP        
		| D3DPTADDRESSCAPS_BORDER       
		| D3DPTADDRESSCAPS_INDEPENDENTUV
		| D3DPTADDRESSCAPS_MIRRORONCE   ;

	pCaps->VolumeTextureAddressCaps = pCaps->TextureAddressCaps;

	pCaps->LineCaps = D3DLINECAPS_TEXTURE  
					  | D3DLINECAPS_ZTEST    
					  | D3DLINECAPS_BLEND    
					  | D3DLINECAPS_ALPHACMP 
					  | D3DLINECAPS_FOG      
					  | D3DLINECAPS_ANTIALIAS;

	pCaps->MaxTextureWidth = 1<<12;
	pCaps->MaxTextureHeight = 1<<12;
	pCaps->MaxVolumeExtent = 1<<12;
	pCaps->MaxTextureRepeat = 1<<16;
	pCaps->MaxTextureAspectRatio = 1<<12;
	pCaps->MaxAnisotropy = 16;
	pCaps->MaxVertexW = 1E+010;

	pCaps->GuardBandLeft = -1E+008;
	pCaps->GuardBandTop = -1E+008;
	pCaps->GuardBandRight = 1E+008;
	pCaps->GuardBandBottom = 1E+008;

	pCaps->ExtentsAdjust = 0;

	pCaps->StencilCaps = D3DSTENCILCAPS_KEEP    
						 | D3DSTENCILCAPS_ZERO    
						 | D3DSTENCILCAPS_REPLACE 
						 | D3DSTENCILCAPS_INCRSAT 
						 | D3DSTENCILCAPS_DECRSAT 
						 | D3DSTENCILCAPS_INVERT  
						 | D3DSTENCILCAPS_INCR    
						 | D3DSTENCILCAPS_DECR    
						 | D3DSTENCILCAPS_TWOSIDED;

	pCaps->FVFCaps = D3DFVFCAPS_TEXCOORDCOUNTMASK 
					 | D3DFVFCAPS_DONOTSTRIPELEMENTS
					 | D3DFVFCAPS_PSIZE             ;

	pCaps->TextureOpCaps = D3DTEXOPCAPS_DISABLE                  
						   | D3DTEXOPCAPS_SELECTARG1               
						   | D3DTEXOPCAPS_SELECTARG2               
						   | D3DTEXOPCAPS_MODULATE                 
						   | D3DTEXOPCAPS_MODULATE2X               
						   | D3DTEXOPCAPS_MODULATE4X               
						   | D3DTEXOPCAPS_ADD                      
						   | D3DTEXOPCAPS_ADDSIGNED                
						   | D3DTEXOPCAPS_ADDSIGNED2X              
						   | D3DTEXOPCAPS_SUBTRACT                 
						   | D3DTEXOPCAPS_ADDSMOOTH                
						   | D3DTEXOPCAPS_BLENDDIFFUSEALPHA        
						   | D3DTEXOPCAPS_BLENDTEXTUREALPHA        
						   | D3DTEXOPCAPS_BLENDFACTORALPHA         
						   | D3DTEXOPCAPS_BLENDTEXTUREALPHAPM      
						   | D3DTEXOPCAPS_BLENDCURRENTALPHA        
						   | D3DTEXOPCAPS_PREMODULATE              
						   | D3DTEXOPCAPS_MODULATEALPHA_ADDCOLOR   
						   | D3DTEXOPCAPS_MODULATECOLOR_ADDALPHA   
						   | D3DTEXOPCAPS_MODULATEINVALPHA_ADDCOLOR
						   | D3DTEXOPCAPS_MODULATEINVCOLOR_ADDALPHA
						   | D3DTEXOPCAPS_BUMPENVMAP               
						   | D3DTEXOPCAPS_BUMPENVMAPLUMINANCE      
						   | D3DTEXOPCAPS_DOTPRODUCT3              
						   | D3DTEXOPCAPS_MULTIPLYADD              
						   | D3DTEXOPCAPS_LERP                     ;

	pCaps->MaxTextureBlendStages = 16;
	pCaps->MaxSimultaneousTextures = 16;

	pCaps->VertexProcessingCaps = D3DVTXPCAPS_TEXGEN              
								  | D3DVTXPCAPS_MATERIALSOURCE7     
								  | D3DVTXPCAPS_DIRECTIONALLIGHTS   
								  | D3DVTXPCAPS_POSITIONALLIGHTS    
								  | D3DVTXPCAPS_LOCALVIEWER         
								  | D3DVTXPCAPS_TWEENING            
								  | D3DVTXPCAPS_TEXGEN_SPHEREMAP    
								  | D3DVTXPCAPS_NO_TEXGEN_NONLOCALVIEWER;

	pCaps->MaxActiveLights	= 32;
	pCaps->MaxUserClipPlanes = 32;
	pCaps->MaxVertexBlendMatrices = 256;
	pCaps->MaxVertexBlendMatrixIndex = pCaps->MaxVertexBlendMatrices+1;
	pCaps->MaxPointSize = 3.0f;
	pCaps->MaxPrimitiveCount = 0xffff-1;
	pCaps->MaxVertexIndex = 0xffffffff;
	pCaps->MaxStreams = 16;
	pCaps->MaxStreamStride = 16;
	
	pCaps->VertexShaderVersion = 0x03;
	pCaps->MaxVertexShaderConst = 32;
	pCaps->PixelShaderVersion = 0x03;
	pCaps->PixelShader1xMaxValue = 8.0f;

	pCaps->DevCaps2 = D3DDEVCAPS2_STREAMOFFSET                      
					  | D3DDEVCAPS2_DMAPNPATCH                        
					  | D3DDEVCAPS2_ADAPTIVETESSRTPATCH               
					  | D3DDEVCAPS2_ADAPTIVETESSNPATCH                
					  | D3DDEVCAPS2_CAN_STRETCHRECT_FROM_TEXTURES     
					  | D3DDEVCAPS2_PRESAMPLEDDMAPNPATCH              
					  | D3DDEVCAPS2_VERTEXELEMENTSCANSHARESTREAMOFFSET;


	pCaps->MaxNpatchTessellationLevel = 32;
	pCaps->MasterAdapterOrdinal = 0;
	pCaps->AdapterOrdinalInGroup = 0;
	pCaps->AdapterOrdinal = 0;
	pCaps->NumberOfAdaptersInGroup = 1;

	pCaps->DeclTypes = D3DDTCAPS_UBYTE4     
					   | D3DDTCAPS_UBYTE4N    
					   | D3DDTCAPS_SHORT2N    
					   | D3DDTCAPS_SHORT4N    
					   | D3DDTCAPS_USHORT2N   
					   | D3DDTCAPS_USHORT4N   
					   | D3DDTCAPS_UDEC3      
					   | D3DDTCAPS_DEC3N      
					   | D3DDTCAPS_FLOAT16_2  
					   | D3DDTCAPS_FLOAT16_4  ;
	
	pCaps->NumSimultaneousRTs = 1;
	pCaps->StretchRectFilterCaps = pCaps->VertexTextureFilterCaps;

	pCaps->VS20Caps.Caps = D3DVS20CAPS_PREDICATION;
	
	pCaps->VS20Caps.DynamicFlowControlDepth = 24;
	pCaps->VS20Caps.NumTemps = 32;
	pCaps->VS20Caps.StaticFlowControlDepth = 4;

					  
	pCaps->PS20Caps.Caps	= D3DPS20CAPS_ARBITRARYSWIZZLE       
					  | D3DPS20CAPS_GRADIENTINSTRUCTIONS   
					  | D3DPS20CAPS_PREDICATION            
					  | D3DPS20CAPS_NODEPENDENTREADLIMIT   
					  | D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT  	;
	pCaps->PS20Caps.DynamicFlowControlDepth = 24;
	pCaps->PS20Caps.NumInstructionSlots = 512;
	pCaps->PS20Caps.NumTemps = 32;
	pCaps->PS20Caps.StaticFlowControlDepth = 4;
	
	pCaps->VertexTextureFilterCaps = pCaps->VertexTextureFilterCaps;

	pCaps->MaxVShaderInstructionsExecuted = 32768;
	pCaps->MaxPShaderInstructionsExecuted = 32768;
	pCaps->MaxVertexShader30InstructionSlots = 32768;
	pCaps->MaxPixelShader30InstructionSlots = 32768;
					  








	return HRESULT_Proc(S_OK);	     
};								  