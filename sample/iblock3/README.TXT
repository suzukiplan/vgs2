[Invader Block 3]
Invader Block 3�́A�{�[���ŃC���x�[�_�[��|���A�N�V�����Q�[���ł��B
SUZUKI PLAN���AGameDaddy�Ή��Q�[�����e�Ƃ��ĊJ�����܂����B
GooglePlay�AAppStore�Ō��J���ł��B

(Android��)
https://play.google.com/store/apps/details?id=com.suzukiplan.IBLOCK3

(iPhone��)
https://itunes.apple.com/us/app/invader-block-3/id838888746?mt=8

[�{�T���v���T�v]
�{�T���v���́AInvader Block 3�̃\�[�X�R�[�h��ROM�f�[�^�ł��B
������p���āAWindows��Android�̃C���[�W���r���h����菇���L���܂��B
���̎菇�����{����O�ɁAGameDaddy�{�O��\�t�g�E�F�A�iWindows��Android�j���C���X�g�[�����Ă����K�v������܂��B
https://github.com/suzukiplan/vgs2/wiki/2.-%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E6%96%B9%E6%B3%95

[�r���h�菇]
Visual C++�̃R�}���h�v�����v�g���N�����Ď��̃R�}���h�����s
> c:
> cd c:\VGS2\sample
��L�̏�Ԃ���AWindows��Android��Invader Block 3���r���h����܂ł̎菇���L���܂��B

(1)vgs2mkpj�œK���ȃv���W�F�N�g�����
> vgs2mkpj suzukiplan IBLOCK3 c:\myprj\IBLOCK3

(2)game.c��z�u
> copy game.c c:\myprj\IBLOCK3\src\game.c

(3)ROMDATA.BIN��z�u
> copy ROMDATA.BIN c:\myprj\IBLOCK3\windows\ROMDATA.BIN
> copy ROMDATA.BIN c:\myprj\IBLOCK3\android\res\raw\romdata.bin

(4)c:\myprj\IBLOCK3\windows\makefile ����romdata��makefile���L�b�N���Ă���ӏ������
before) all: MKROM $(RELDIR) $(PROJECT).EXE
 after) all:       $(RELDIR) $(PROJECT).EXE

��L�菇�����{��ɁAWindows�AAndroid�Ńr���h�����s�����OK�ł��B

[Windows�ł̃r���h�����s]
> cd c:\myprj\IBLOCK3\windows\
> nmake
> IBLOCK3.EXE

[Android�ł̃r���h�����s]
cygwin:
$ cd /cygdrive/c/myprj/IBLOCK3/android/jni
$ ndk-build

Visual C++�̃R�}���h�v�����v�g:
> c:
> cd c:\myprj\IBLOCK3\android
> ant debug
> adb install -r bin\IBLOCK3-debug.apk
�����K�ł�Invader Block 3���C���X�g�[������Ă���[�����ƁAadb install�Ɏ��s���܂��B