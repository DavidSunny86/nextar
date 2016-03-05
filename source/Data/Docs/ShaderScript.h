/*! @page ShaderScript Shader script documentation
 	@tableofcontents
 	@section Commands
 	
 	@subsection shader <name>
 	Starts a new shader declaration
 	
 		@subsection tags tag1 tag2;
 		Adds tags to the shader helping it to identify the render queue it belongs to.
 		Valid tags are:
 			background
 			deferred
 			debug
 			deferred-lighting
 			forward
 			overlay
 			compositor
 			translucency
 		
 		@subsection pass <name>
 		Adds a pass to the shader.
 	
 			@subsection depth-stencil-state <state> <arg1> ...;
 			Sets the current depth stencil state. 
 			Valid values are:
 			disable: Disables both stencil and depth state.
 			default: Enables depth test (with less-equal) and write, but disables stencil test.
 			 
	 			@subsection depth-state:
	 			Sets the depth state.
	 			Valid values are:
	 			test-and-write: The default write, test with less-equal state.
	 			default: same as test-and-write.
	 			disable: No depth test and write.
	 			advanced test write compare-function: The parameters are as follows-
	 				test: boolean, true if depth test is enabled.
	 				write: boolean, true if depth write is enabled.
	 				compare-function: Valid values are -
	 					less
	 					less-equal
 					
 	
 	