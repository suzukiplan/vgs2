all:\
	..\bin \
	..\bin\vgs2mml.exe \
	..\bin\vgs2bmp.exe \
	..\bin\vgs2mkpj.exe \
	..\bin\vgs2rom.exe \
	..\bin\vgs2wav.exe

clean:
	-@rmdir /s /q ..\bin
	-@del vgs2bmp.obj
	-@del vgs2bmp.exe
	-@del vgs2mkpj.obj
	-@del vgs2mkpj.exe
	-@del vgs2rom.obj
	-@del vgs2rom.exe
	-@del vgs2wav.obj
	-@del vgs2wav.exe
	-@del vgs2mml.obj
	-@del vgs2mml.exe

..\bin:
	mkdir ..\bin

T=vgs2bmp
..\bin\$(T).exe: $(T).c
	CL /MT $(T).c
	copy $(T).exe ..\bin\$(T).exe

T=vgs2mkpj
..\bin\$(T).exe: $(T).c
	CL /MT $(T).c advapi32.lib
	copy $(T).exe ..\bin\$(T).exe

T=vgs2rom
..\bin\$(T).exe: $(T).c
	CL /MT $(T).c
	copy $(T).exe ..\bin\$(T).exe

T=vgs2wav
..\bin\$(T).exe: $(T).c
	CL /MT $(T).c
	copy $(T).exe ..\bin\$(T).exe

T=vgs2mml
..\bin\$(T).exe: $(T).c
	CL /MT $(T).c
	copy $(T).exe ..\bin\$(T).exe

