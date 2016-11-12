/*! @page ShaderScript Shader script documentation
 	@tableofcontents
 	@section Commands
 	
 	@subsection shader <name> ...;
 	Starts a new shader declaration
 	
 		@subsection tags tag1 tag2;
 		Adds tags to the shader helping it to identify the render queue it belongs to.
 		Valid tags are:-
 			background
 			deferred
 			debug
 			deferred-lighting
 			forward
 			overlay
 			compositor
 			translucency
 		
 		@subsection pass <name> ...;
 		Adds a pass to the shader.

			@subsection cbuffer <buffer-name-0> <buffer-name-1> ...;
			Append constant buffer declaration to the shaders. 
			The buffers files must be present under <data directory>/Scripts/ConstBuffers.
 	
 			@subsection depth-stencil-state <state> <arg1> ...;
 			Sets the current depth stencil state. 
 			Valid values are:-
 			disable:- Disables both stencil and depth state.
 			default:- Enables depth test (with less-equal) and write, but disables stencil test.
 			 
	 			@subsection depth-state <value> ...;
	 			Sets the depth state.
	 			value:- Valid values are:-
	 				test-and-write:- The default write, test with less-equal state.
	 				default:- same as test-and-write.
	 				disable:- No depth test and write.
	 				advanced <test> <write> <compare-function>:-
	 					test:- boolean, true if depth test is enabled.
	 					write:- boolean, true if depth write is enabled.
	 					compare-function:- Valid values are -
	 						less
	 						less-equal

				@subsection stencil-state <value> ...;
				Sets the stencil state.
				value:- Valid values are:-
					disable: Disabled.
					default: Disabled.
					advanced <front/back> <stencil-func> <stencil-pass-op> <stencil-fail-op> <depth-pass-op> <stencil-mask> <stencil-ref>:-
						front/back:- Indicates if the test should be set for front or back face.
						stencil-func:- default, always, equal, greater, greater-equal, less, less-equal, never, not-equal.
						stencil-pass-op, stencil-fail-op, depth-pass-op:- decrement, decrement-saturate, increment, increment-saturate, invert, keep, replace, zero.
						stencil-mask, stencil-ref:- unsigned long values.


			@subsection texture-state ...;
			Declare texture states.

				@subsection sampler <name> <value> ...;
				name:- Can be any name given to the sampler.
				value:- Valid values are:-
					<filter>-<wrap-mode>:- Formatted sampling state.
						filter:- nearest, linear.
						wrap-mode:- default, border, clamp, mirror, repeat.
					advanced:- Allows inner commands.
					
					@subsection address <u-address> <v-address> <w-address>;
						x-address:- default, border, clamp, mirror, repeat.
					
					@subsection anisotropy <value>;
						Integer value.

					@subsection texture-lod <min> <max> <bias>;
						Floating point values.

					@subsection compare <compare-function>;
						compare-function:- default, always, equal, greater, greater-equal, less, less-equal, never, not-equal.

					@subsection filter <min-filter> <mag-filter>;
						min-filter:- default, linear, linear-mip-linear, linear-mip-nearest, nearest, nearest-mip-linear, nearest-mip-nearest.
						mag-filter:- default, linear, nearest.

				@subsection unit <name:semantic> <sampler> <option-name> <context:type>;
				Declare a texture unit using a certain sampler.
					name:semantic:- Variable name, as used in shaders and associated semantic if any.
					sampler:- Any previously declared sampler name.
					option-name:- If declaring value for this variable in a material should enable certain option, the option name should be provided here.
					context:type:- 'context' is a tag, type referes to either- object, material, frame, pass, view.
			
			@subsection blend-state <value>...;
			Set blend sate. 
			Valid values are:-
				default, disable:- Disables blending.

				@subsection target <type>...;
				Set blend state for the next render target.
				Valid values are:-
					pre-multiplied-blending:- If source is already multiplied with alpha.
					alpha-blending:- source-alpha * source-color + (1-source-alpha) * dest-color
					advanced <blend-source-color> <blend-dest-color> <color-op> <blend-source-alpha> <blend-dest-alpha> <alpha-op>:-
						blend-source-color, blend-dest-color, blend-source-alpha, blend-dest-alpha:- 
							dest-alpha, dest-color, inv-dest-alpha, inv-dest-color, src-alpha, src-color, inv-src-alpha, inv-src-color, one, zero.
						color-op, alpha-op:- add, sub, inv-sub, min, max.

				@subsection alpha-to-coverage <true/false>...;
				Sets alpha to coverage to true by default.

			@subsection program ...;
			Declares program details.
			
				@subsection option <name> <define-0> ...;
				Options can be connected to predefined parameters via names if chosen names are predefined as parameters, then these names
				are evaluated to see if they equate to true or false depending upon their values
				Eg: Texture units evaluate to 0 if they are not set and 1 when they are set
				    Scalar values evaluate to 0 if they are not set and 1 when they are set
				    If the name does not match any parameter previously defined, they are added as boolean shader option.

				@subsection define <define-0> ...;
				Switches on the defined macros in shader.
 	
 	