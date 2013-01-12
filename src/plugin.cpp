// Copyright (c) 2011-2012 Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#include <amx/amx.h>

#include "amx.h"
#include "amxerror.h"
#include "compiler.h"
#include "crashdetect.h"
#include "fileutils.h"
#include "hook.h"
#include "logprintf.h"
#include "os.h"
#include "plugincommon.h"
#include "version.h"

extern "C" int AMXAPI amx_Error(AMX *amx, cell index, int error) {
	if (error != AMX_ERR_NONE) {
		CrashDetect::Get(amx)->HandleExecError(index, error);
	}
	return AMX_ERR_NONE;
}

static int AMXAPI AmxCallback(AMX *amx, cell index, cell *result, cell *params) {
	return CrashDetect::Get(amx)->DoAmxCallback(index, result, params);
}

static int AMXAPI AmxExec(AMX *amx, cell *retval, int index) {
	if (amx->flags & AMX_FLAG_BROWSE) {
		return amx_Exec(amx, retval, index);
	}
	return CrashDetect::Get(amx)->DoAmxExec(retval, index);
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() {
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData) {
	void **exports = reinterpret_cast<void**>(ppData[PLUGIN_DATA_AMX_EXPORTS]);
	::logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];

	void *amx_Exec_ptr = exports[PLUGIN_AMX_EXPORT_Exec];
	void *amx_Exec_sub = Hook::GetTargetAddress(reinterpret_cast<unsigned char*>(amx_Exec_ptr));

	if (amx_Exec_sub == 0) {
		new Hook(amx_Exec_ptr, (void*)AmxExec);
	} else {
		std::string module = fileutils::GetFileName(os::GetModulePathFromAddr(amx_Exec_sub));
		if (!module.empty()) {
			logprintf("  Warning: Runtime error detection will not work during this run because ");
			logprintf("           %s has been loaded before CrashDetect.", module.c_str());
		}
	}

	os::SetExceptionHandler(CrashDetect::OnException);
	os::SetInterruptHandler(CrashDetect::OnInterrupt);

	logprintf("  crashdetect v"PROJECT_VERSION_STRING" is OK.");
	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) {
	int error = CrashDetect::Create(amx)->Load();
	if (error == AMX_ERR_NONE) {
		amx_SetCallback(amx, AmxCallback);
	}
	return error;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) {
	int error = CrashDetect::Get(amx)->Unload();
	CrashDetect::Destroy(amx);
	return error;
}
