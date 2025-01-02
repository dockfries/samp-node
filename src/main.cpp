#include <iostream>
#include <fstream>
#include <algorithm>
#include "callbacks.hpp"
#include "events.hpp"
#include "amxhandler.hpp"
#include "nodeimpl.hpp"
#include <sampgdk.h>
#include "common.hpp"
#include "config.hpp"

logprintf_t logprintf;

const AMX_NATIVE_INFO native_list[] =
{
	{ "SAMPNode_CallEvent", sampnode::event::pawn_call_event },
	{ 0, 0 }
};

PLUGIN_EXPORT bool PLUGIN_CALL OnPublicCall(AMX* amx, const char* name, cell* params, cell* retval)
{
	if (sampnode::js_calling_public)
		return true;

	auto iter = sampnode::events.find(name);
	if (iter != sampnode::events.end())
		iter->second->call(amx, params, retval);

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

PLUGIN_EXPORT bool PLUGIN_CALL Load(void** ppData)
{
	logprintf = (logprintf_t)(ppData[PLUGIN_DATA_LOGPRINTF]);
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];

	sampnode::Config mainConfig;

	if (!mainConfig.ParseFile("samp-node"))
	{
		L_ERROR << "Unable to load samp-node config file, you need to have samp-node.json in root directory"
			<< std::endl << "\tPlease read about samp-node config file in our wiki pages at https://github.com/AmyrAhmady/samp-node/wiki";
		return false;
	}

	const sampnode::Props_t& mainConfigData = mainConfig.ReadAsMainConfig();

	Log::Init(mainConfigData.log_level);

	sampgdk::Load(ppData);
	sampnode::callback::init();
	sampnode::nodeImpl.Initialize(mainConfigData);
	sampnode::nodeImpl.LoadResource("main");
	return true;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX* amx)
{
	sampnode::amx::load(amx);
	if (!sampnode::amx::realAMX)
	{
		cell amx_addr{};
		if (amx_FindPubVar(amx, "_polyfill_is_gamemode", &amx_addr) == AMX_ERR_NONE)
		{
			cell *phys_addr{};
			int get_addr_ret = amx_GetAddr(amx, amx_addr, &phys_addr);
			if (get_addr_ret == AMX_ERR_NONE && static_cast<bool>(*phys_addr))
			{
				sampnode::amx::realAMX = amx;
			}
		}
	}
	return amx_Register(amx, native_list, -1);;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	sampgdk::Unload();
	sampnode::nodeImpl.Stop();
	return;
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX* amx)
{
	sampnode::amx::unload(amx);
	if (amx == sampnode::amx::realAMX)
	{
		sampnode::amx::realAMX = NULL;
	}
	return 1;
}