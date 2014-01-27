

Material Dummy {
  Use Default:DummyShader "${Data}/Shaders/DummyShaderScript.s" Switches:1,2;
  Param float:ambientOcclusionFactor 3.001;
  Param texture:diffuseTexture "Default:Ogre, ${Data}/Textures/ogre.dds, (1,1), (0,0)";
}

DummyShader:
