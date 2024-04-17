#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	WEAK symbol<LRESULT WINAPI(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)> game_winmain{ 0x1401F5260 };
	WEAK symbol<bool()> init_avs{ 0x1401F46B0 };

	typedef uint32_t avs_file_t;

	WEAK avs_function<avs_file_t(const char* name, uint16_t mode, int flags)> avs_fs_open{ "XCgsqzn000004e" };
	WEAK avs_function<int(const char* sname, const char* dname)> avs_fs_copy{ "XCgsqzn0000065" };
	WEAK avs_function<void(avs_file_t file)> avs_fs_close{ "XCgsqzn0000055" };
	WEAK avs_function<int(avs_file_t file, struct avs_stat* stat)> avs_fs_fstat{ "XCgsqzn0000062" };
	WEAK avs_function<int(const char* path, struct avs_stat* stat)> avs_fs_lstat{ "XCgsqzn0000063" };
	WEAK avs_function<int(avs_file_t file, long offset, int origin)> avs_fs_lseek{ "XCgsqzn000004f" };
	WEAK avs_function<size_t(avs_file_t file, const char* data, uint32_t data_size)> avs_fs_read{ "XCgsqzn0000051" };
	WEAK avs_function<avs_file_t(const char* path)> avs_fs_opendir{ "XCgsqzn000005c" };
	WEAK avs_function<const char* (avs_file_t dir)> avs_fs_readdir{ "XCgsqzn000005d" };
	WEAK avs_function<void(avs_file_t dir)> avs_fs_closedir{ "XCgsqzn000005e" };
	WEAK avs_function<int(const char* mountpoint, const char* fsroot, const char* fstype, void* data)> avs_fs_mount{ "XCgsqzn000004b" };
}
