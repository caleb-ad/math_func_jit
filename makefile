all : main.cpp
	cl -EHsc main.cpp

dll_test.dll : dll_test.obj
	cl -Fe:dll_test.dll dll_test.obj -link -DLL

dll_test.obj : dll_test.cpp
	cl -c -Fo:dll_test.obj -EHsc dll_test.cpp

clean:
	rm *.dll
	rm *.obj
	rm *.lib
	rm *.exp