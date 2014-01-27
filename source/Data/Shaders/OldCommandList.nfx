
Shader DummyShader {

	DepthStencilState {
		// Params: Test Write Compare
		DetphState true true lessEq;
		// Params:   Test
		StencilTest true;
		// Params:   ColorMask  FrontFunc  StencilFailOp StencilPassOp DepthPassOp
		StencilFrontOp rgba greaterEq decSat dec invert;
		// Params:   ColorMask  BackFunc  StencilFailOp StencilPassOp DepthPassOp
		StencilBackOp rgba less inc incSat replace;
	}

	BlendState {
		// Params: Enable AlphaToCoverage NumberOfRenderTargets
		Blend true true 1;
		// Params: TargetNumber Enable ColorMask SourceColor DestColor ColorOp SourceAlpha DestAlpha AlphaOp
		Target 0 true rgba srcAlpha destAlpha add srcAlpha destCol mul;
		Target 1 false;
	}

	RasterState {
		// Params: FillMode CullTriangle ClockWiseTriangleOrientation
		Raster solid back true;
		// Params: Enable
		Scissor true;
		// Params: UsingMultisampling UsingLineAA
		AntiAliasing true true;
		// Params: Enable
		DepthClip true;
		// Params: ConstantBias SlopeScaledDepthBias DepthBiasClamp
		DepthBias 0.01 0.01 0.01;
	}

	TextureState {
		// Params: SamplerName MinFilter MagFilter UWarp VWarp WWarp Comparison MaxAnisotropy MinLod MaxLod LodBias BorderColor[r g b a]
		Unit shadowTexture nearest nearest warp warp warp lessEq 100 0 1.0 0.1 [0.3 0.1 0.4 0.1];
		Unit diffuseTexture nearest;
		Unit detailTexture nearest;
	}

	Program {
		// [ Material Property Table ]
		// [ "Occlusion Factor": float value },
		Param float:ambientOcclusionFactor "Occlusion Factor"  0.44;
		Param texture:detailTexture ""  ${Data}/Textures/BlankTexture.png;
		Param texture:diffuseTexture "" ${Data}/Textures/BlankTexture.png;

		Option DETAIL_TEXTURE "Use Detail Texture" true "Turns on detail texturing.";
		Option SOFT_SHADOWING "Use Soft Texture" true "Turns on soft shadowing.";
		Option PCF_FILTER "Use PCF Filtering" true "Turns on PCF filtering.";

		VertexProgram DebugShader.vert;
		PixelProgram Name:DebugShader File:DebugShader.frag;
	}
}}
