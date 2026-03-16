#include <iostream>
#include <fstream>
#include <algorithm>
#include "common.hpp"
#include "callbacks.hpp"
#include "events.hpp"
#include "amxhandler.hpp"
#include "nodeimpl.hpp"
#include "sampgdk.h"
#include "config.hpp"

const AMX_NATIVE_INFO native_list[] =
		{
				{"SAMPNode_CallEvent", sampnode::event::pawn_call_event},
				{0, 0}};

PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX *amx, const char *name, cell *params, cell *retval)
{
	if (sampnode::js_calling_public)
		return true;

	auto iter = sampnode::events.find(name);
	if (iter != sampnode::events.end())
		iter->second->call(amx, params, retval, false);
	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
	sampgdk::ProcessTick();
	sampnode::nodeImpl.Tick();
	return;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	sampnode::Config mainConfig;

	if (!mainConfig.ParseFile("samp-node"))
	{
		L_ERROR << "Unable to load samp-node config file, you need to have samp-node.json in root directory"
						<< std::endl
						<< "\tPlease read about samp-node api at https://github.com/dockfries/samp-node";
		return false;
	}

	const sampnode::Props_t &mainConfigData = mainConfig.ReadAsMainConfig();

	Log::Init(mainConfigData.log_level);

	sampgdk::Load(ppData);
	sampnode::nodeImpl.Initialize(mainConfigData);
	sampnode::nodeImpl.LoadResource("main");
	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
	sampnode::amx::load(amx);
	return amx_Register(amx, native_list, -1);
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	sampgdk::Unload();
	sampnode::nodeImpl.Stop();
	return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
	sampnode::amx::unload(amx);
	return 1;
}