mfj.exe: math_func_jit.cpp
	cl -W3 -wd4996 -EHsc -Fe:mfj.exe math_func_jit.cpp FuncInterp.lib

clean:
	rm *.dll; rm *.obj; rm *.exe; rm my_func.cpp; rm my_func.cpp.bat;
