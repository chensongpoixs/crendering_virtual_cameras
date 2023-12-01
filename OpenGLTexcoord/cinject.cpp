#include "cinject.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "detours.h"
#include <detours.h>
//detours.lib;
#pragma comment(lib, "detours.lib")
#define WARNING_EX_LOG printf

#define SYSTEM_LOG  printf

namespace chen {

	static HMODULE user32dll = NULL;

	typedef HANDLE(WINAPI* PFN_CreateFileW)(_In_ LPCWSTR lpFileName,
		_In_ DWORD dwDesiredAccess,
		_In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		_In_ DWORD dwCreationDisposition,
		_In_ DWORD dwFlagsAndAttributes,
		_In_opt_ HANDLE hTemplateFile);

	PFN_CreateFileW RealCreateFileW = NULL;

	static HANDLE WINAPI hook_CreateFileW(_In_ LPCWSTR lpFileName,
		_In_ DWORD dwDesiredAccess,
		_In_ DWORD dwShareMode,
		_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		_In_ DWORD dwCreationDisposition,
		_In_ DWORD dwFlagsAndAttributes,
		_In_opt_ HANDLE hTemplateFile)
	{ 
		printf("[%s][%d][lpFileName = %ws]\n",__FUNCTION__, __LINE__, lpFileName);
		return RealCreateFileW(  lpFileName,
			 dwDesiredAccess,
			 dwShareMode,
			  lpSecurityAttributes,
			  dwCreationDisposition,
			  dwFlagsAndAttributes,
			  hTemplateFile);
		 
	}
	/*WINBASEAPI
		HANDLE
		WINAPI
		CreateFileW(
			_In_ LPCWSTR lpFileName,
			_In_ DWORD dwDesiredAccess,
			_In_ DWORD dwShareMode,
			_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			_In_ DWORD dwCreationDisposition,
			_In_ DWORD dwFlagsAndAttributes,
			_In_opt_ HANDLE hTemplateFile
		);*/
	static inline HMODULE get_system_module(const char* system_path, const char* module)
	{
		char base_path[MAX_PATH];

		strcpy(base_path, system_path);
		strcat(base_path, "\\");
		strcat(base_path, module);
		return GetModuleHandleA(base_path);
	}

	bool init_inject()
	{
		char system_path[MAX_PATH] = { 0 };

		UINT ret = GetSystemDirectoryA(system_path, MAX_PATH);
		if (!ret)
		{
			printf("Failed to get windows system path: %lu\n", GetLastError());
			//return false;
		}
		printf("[system_path = %s] ok ", system_path);
		// hook mouse move  GetRawInputData
		user32dll = get_system_module(system_path, "Kernel32.dll");
		if (!user32dll)
		{
			WARNING_EX_LOG(" REGISTER mouse  failed !!!");
			//return false;
		}

		void* get_CreateFileW_proc = GetProcAddress(user32dll, "CreateFileW");
		//SYSTEM_LOG("REGISTER  mouse ok !!!");
		//void* get_raw_input_data_proc = GetProcAddress(user32dll, "GetRawInputData");
		//void* get_cursor_pos_proc = GetProcAddress(user32dll, "GetCursorPos");
		//void* set_cursor_pos_proc = GetProcAddress(user32dll, "SetCursorPos");
		//void* get_key_state_proc = GetProcAddress(user32dll, "GetKeyState"); //hook_RealGetKeyState
		//void* get_async_key_state_proc = GetProcAddress(user32dll, "GetAsyncKeyState");
		/// <summary>
		/// ///////////////////
		/// </summary>
		/// <returns></returns>
		/*void* register_class_a_proc = GetProcAddress(user32dll, "RegisterClassA");
		void* register_class_w_proc = GetProcAddress(user32dll, "RegisterClassW");



		void* unregister_class_a_proc = GetProcAddress(user32dll, "UnRegisterClassA");
		void* unregister_class_w_proc = GetProcAddress(user32dll, "UnRegisterClassW");*/

		/*if (!get_raw_input_data_proc)
		{
			ERROR_EX_LOG("seatch mouse table not find GetRawInputData !!!");
		}
		else*/
		{
			SYSTEM_LOG("    input device  begin ... ");
			DetourTransactionBegin();
			if (get_CreateFileW_proc)
			{
				RealCreateFileW = (PFN_CreateFileW)get_CreateFileW_proc;
				DetourAttach((PVOID*)&RealCreateFileW,
					hook_CreateFileW);
			}

			/*if (get_raw_input_data_proc)
			{
				RealGetRawInputData = (PFN_GetRawInputData)get_raw_input_data_proc;
				DetourAttach((PVOID*)&RealGetRawInputData,
					hook_get_raw_input_data);
			}
			if (get_cursor_pos_proc)
			{
				RealGetCursorPos = (PFN_GetCursorPos)get_cursor_pos_proc;
				DetourAttach((PVOID*)&RealGetCursorPos,
					hook_get_cursor_pos);
			}
			if (set_cursor_pos_proc)
			{
				RealSetCursorPos = (PFN_SetCursorPos)set_cursor_pos_proc;
				DetourAttach((PVOID*)&RealSetCursorPos,
					hook_set_cursor_pos);
			}
			if (get_async_key_state_proc)
			{
				RealGetAsyncKeyState = (PFN_GetAsyncKeyState)get_async_key_state_proc;
				DetourAttach((PVOID*)&RealGetAsyncKeyState,
					hook_GetAsyncKeyState);
			}
			if (get_key_state_proc)
			{
				RealGetKeyState = (PFN_GetKeyState)get_key_state_proc;
				DetourAttach((PVOID*)&RealGetKeyState,
					hook_GetKeyState);
			}


			if (register_class_a_proc)
			{
				RealRegisterClassA = (PFN_RegisterClassA)register_class_a_proc;
				DetourAttach((PVOID*)&RealRegisterClassA,
					hook_RegisterClassA);
			}

			if (register_class_w_proc)
			{
				RealRegisterClassW = (PFN_RegisterClassW)register_class_w_proc;
				DetourAttach((PVOID*)&RealRegisterClassW,
					hook_RegisterClassW);
			}
			if (unregister_class_a_proc)
			{
				RealUnregisterClassA = (PFN_UnregisterClassA)unregister_class_a_proc;
				DetourAttach((PVOID*)&RealUnregisterClassA,
					hook_RegisterClassA);
			}

			if (unregister_class_w_proc)
			{
				RealUnregisterClassW = (PFN_UnregisterClassW)unregister_class_w_proc;
				DetourAttach((PVOID*)&RealUnregisterClassW,
					hook_RegisterClassW);
			}*/

			SYSTEM_LOG("   input end  ... ");
			const LONG error = DetourTransactionCommit();
			const bool success = error == NO_ERROR;
			if (success)
			{
				/*NORMAL_EX_LOG("  input device");
				if (get_raw_input_data_proc)
				{
					NORMAL_EX_LOG("  input device");
				}
				if (get_cursor_pos_proc)
				{
					NORMAL_EX_LOG("  input device");
				}
				if (set_cursor_pos_proc)
				{
					NORMAL_EX_LOG("  input device");
				}
				if (get_async_key_state_proc)
				{
					NORMAL_EX_LOG("input key ");
				}
				if (get_key_state_proc)
				{
					NORMAL_EX_LOG("input key state");
				}*/
				/*if (register_class_a_proc)
				{
					NORMAL_EX_LOG(" input device");
				}
				if (register_class_w_proc)
				{
					NORMAL_EX_LOG(" input device");
				}*/
				//NORMAL_EX_LOG("  input device ");
			}
			else
			{
				/*RealGetRawInputData = NULL;
				RealGetCursorPos = NULL;
				RealSetCursorPos = NULL;
				RealGetAsyncKeyState = NULL;*/
				/*RealRegisterClassA = NULL;
				RealRegisterClassW = NULL;*/
				//ERROR_EX_LOG("Failed to attach  mouse  : %ld", error);
			}
		}
		return true;
	}
}