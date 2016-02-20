/**
 * @remarks
 * Pass script command documentation.
 * ==================================
 *
 * Commands:
 *
 * create-buffer <name> [width] [height]:
 *  Creates a multi render target using sub-commands of given width and height (optional).
 *  Width and height can be specified in terms of current viewport target dimensions as -
 *  view-x-<factor>
 *  factor - Can be any floating point value.
 *
 *  Commands:
 *  	target [depth/color] <format> [boolean:as texture]:
 *  		Creates a render target and associates it with the name given. Adds this target to the multi-render target.
 *  		The boolean parameter indicates if the target should be a render buffer (false) or texture (true).
 *  		@note Name of a target is always appended to the buffer name with a '.' operator, followed by:
 *  		 color-[index]: For color target.
 *  		 depth: For depth target.
 *  		 Eg: Using 'main.color-0' for a target binding will fetch the 0th color render target for the buffer 'main'.
 *  		@note The definition here will not create a multi-render target if there is just one target definition.
 *
 *  pass <name>:
 *   Adds a pass to the render system.
 *
 *
 * Pass types and commands:
 *
 * BasePass:
 * 	Does not do anything, but handles default commands.
 *
 * 	Commands:
 * 		render-to <target-name>:
 * 			Sets it as current target and renders to it.
 * 			target-name - can be the following in built type:
 * 				viewport - the render target viewport points to.
 * 				last - the render target set last time.
 * 				[autoname].[subbuffer] - for multi render targets, buffer names are appended with their target name.
 * 				Eg: last.color-0
 *
 * 		clean-depth:
 * 			Clears depth buffer if called.
 * 		clean-color <index> <value>:
 * 			Clears color buffer at the given index (for m.r.t.) and with the given color value (rgba list, or color name).
 *
 * MaterialPass:
 * 	Base for material based passes (Compositor, Copy, etc).
 *
 * 	Commands:
 * 		bind-texture-material <target-name> <material-parameter-name>:
 * 			Binds the given target as the material parameter.
 *
 * Clean:
 * 	Cleans the buffer it renders to.
 *
 * 	Commands:
 *
 * */
