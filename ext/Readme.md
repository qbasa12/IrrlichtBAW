# What IrrExtensions are

Stuff which is not 100% necessary for a GPGPU engine, or could have 3 or 4 possible implementations
none of which would be optimal for all circumstances such as shadows or deferred rendering.

General rules for extensions are:
1) **Put a LICENSE.md file at the root of your extension directory**
2) Put it in the irr::ext::YourExtension namespace
3) Make it compile only through inclusion with end-user projects, or **exceptionally** as a library with CMake.
4) Include your own files (even in your own .c/cpp files) by specifying their
   paths relative to the irrlicht root (see rule 2)
5) If the extension requires higher capability hardware than IrrlichtBAW then
   provide static function to check whether the hardware supports it at runtime
6) Include a short README.md outlining external API, but not as documentation
7) Document your extension headers Doxygen-style


Current List of Extensions:
+ AutoExposure
+ Compute Shader 2D Box Blur
+ Full-screen Triangle for Fragment Shader Post Processing
+ Debug Line Draw using streaming buffer
