#include <vector>
#include <string>
#ifndef WIN32
#include <cstring>
#endif

#include "logger.hpp"
#include "resource.hpp"
#include "events.hpp"
#include "callbacks.hpp"
#include "amx/amx.h"
#include "amxhandler.hpp"
#include "sampgdk.h"

namespace sampnode
{
	bool js_calling_public = false;
	static const std::vector<callback::data> sampCallbacks =
	{
		{ "OnGameModeInit", ""},
		{ "OnGameModeExit", ""},
		{ "OnFilterScriptInit", ""},
		{ "OnFilterScriptExit", ""},
		{ "OnPlayerConnect", "i"},
		{ "OnPlayerDisconnect", "ii"},
		{ "OnPlayerSpawn", "i"},
		{ "OnPlayerDeath", "iii"},
		{ "OnVehicleSpawn", "i"},
		{ "OnVehicleDeath", "ii"},
		{ "OnPlayerText", "is"},
		{ "OnPlayerCommandText", "is"},
		{ "OnPlayerRequestClass", "ii"},
		{ "OnPlayerEnterVehicle", "iii"},
		{ "OnPlayerExitVehicle", "ii"},
		{ "OnPlayerStateChange", "iii"},
		{ "OnPlayerEnterCheckpoint", "i"},
		{ "OnPlayerLeaveCheckpoint", "i"},
		{ "OnPlayerEnterRaceCheckpoint", "i"},
		{ "OnPlayerLeaveRaceCheckpoint", "i"},
		{ "OnRconCommand", "s"},
		{ "OnPlayerRequestSpawn", "i"},
		{ "OnObjectMoved", "i"},
		{ "OnPlayerObjectMoved", "ii"},
		{ "OnPlayerPickUpPickup", "ii"},
		{ "OnVehicleMod", "iii"},
		{ "OnEnterExitModShop", "iii"},
		{ "OnVehiclePaintjob", "iii"},
		{ "OnVehicleRespray", "iiii"},
		{ "OnVehicleDamageStatusUpdate", "ii"},
		{ "OnUnoccupiedVehicleUpdate", "iiiffffff"},
		{ "OnPlayerSelectedMenuRow", "ii"},
		{ "OnPlayerExitedMenu", "i"},
		{ "OnPlayerInteriorChange", "iii"},
		{ "OnPlayerKeyStateChange", "iii"},
		{ "OnRconLoginAttempt", "ssi"},
		{ "OnPlayerUpdate", "i"},
		{ "OnPlayerStreamIn", "ii"},
		{ "OnPlayerStreamOut", "ii"},
		{ "OnVehicleStreamIn", "ii"},
		{ "OnVehicleStreamOut", "ii"},
		{ "OnActorStreamIn", "ii"},
		{ "OnActorStreamOut", "ii"},
		{ "OnDialogResponse", "iiiis"},
		{ "OnPlayerTakeDamage", "iifii"},
		{ "OnPlayerGiveDamage", "iifii"},
		{ "OnPlayerGiveDamageActor", "iifii"},
		{ "OnPlayerClickMap", "ifff"},
		{ "OnPlayerClickTextDraw", "ii"},
		{ "OnPlayerClickPlayerTextDraw", "ii"},
		{ "OnIncomingConnection", "isi"},
		{ "OnTrailerUpdate", "ii"},
		{ "OnVehicleSirenStateChange", "iii"},
		{ "OnPlayerFinishedDownloading", "ii"},
		{ "OnPlayerRequestDownload", "iii"},
		{ "OnPlayerClickPlayer", "iii"},
		{ "OnPlayerEditObject", "iiiiffffff"},
		{ "OnPlayerEditAttachedObject", "iiiiifffffffff"},
		{ "OnPlayerSelectObject", "iiiifff"},
		{ "OnPlayerWeaponShot", "iiiifff"},
		{ "OnClientCheckResponse", "iiii"},
		{ "OnScriptCash", "iii" }
	};

	void callback::init()
	{
		for (auto& callback : sampCallbacks)
		{
			event::register_event(callback.name, callback.param_types);
		}
	}

	void callback::call(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolateScope(isolate);

		auto _context = isolate->GetCurrentContext();
		v8::Context::Scope contextScope(_context);

		v8::TryCatch eh(isolate);
		int returnValue = 0;

		if (info.Length() > 0)
		{
			v8::String::Utf8Value str(isolate, info[0]);
			std::string name(*str);

			v8::String::Utf8Value str2(isolate, info[1]);
			std::string format(*str2);

			int k = 2;
			cell _params[32];
			std::vector<void*> params;
			int numberOfStrings = 0;
			int arraySizes[32];
			for (int i = 0; i < static_cast<int>(format.length()); i++)
			{
				switch (format[i])
				{
				case 'i':
				case 'd':
				{
					_params[i] = info[k]->Int32Value(_context).ToChecked();
					params.push_back(static_cast<void*>(&_params[i]));
					k++;
				}
				break;
				case 'f':
				{
					float val = 0.0;
					if (!info[k]->IsUndefined()) val = static_cast<float>(info[k]->NumberValue(_context).ToChecked());
					_params[i] = amx_ftoc(val);
					params.push_back(static_cast<void*>(&_params[i]));
					k++;
				}
				break;
				case 's':
				{
					v8::String::Utf8Value _str(isolate, info[k]);
					const char* str(*_str);
					size_t slen = strlen(str);
					char* mystr = new char[slen + 1];
					for (size_t x = 0; x < slen; x++)
					{
						mystr[x] = str[x];
					}
					mystr[slen] = '\0';
					params.push_back(static_cast<void*>(mystr));
					numberOfStrings++;
					k++;
				}
				break;
				case 'a':
				{
					if (!info[k]->IsArray())
					{
						info.GetReturnValue().Set(false);
						L_ERROR << "callPublic: '" << name << "', parameter " << k << "must be an array";
						return;
					}

					v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(info[k]);
					size_t size = a->Length();

					cell* value = new cell[size];

					for (size_t b = 0; b < size; b++)
					{
						value[b] = a->Get(_context, b).ToLocalChecked()->Int32Value(_context).ToChecked();
					}

					params.push_back(static_cast<void*>(value));
					arraySizes[i] = size;
					numberOfStrings++;
					k++;
				}
				break;
				case 'v':
				{
					if (!info[k]->IsArray())
					{
						info.GetReturnValue().Set(false);
						L_ERROR << "callPublic: '" << name << "', parameter " << k << "must be an array";
						return;
					}

					v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(info[k]);
					size_t size = a->Length();

					cell* value = new cell[size];

					for (size_t b = 0; b < size; b++)
					{
						float val = static_cast<float>(a->Get(_context, b).ToLocalChecked()->NumberValue(_context).ToChecked());
						value[b] = amx_ftoc(val);
					}

					params.push_back(static_cast<void*>(value));
					arraySizes[i] = size;
					numberOfStrings++;
					k++;
				}
				break;
				default:
					break;
				}
			}

			std::vector<cell> amx_addr;
			for (int i = 0; i < numberOfStrings; i++)
			{
				amx_addr.push_back(0);
			}
			numberOfStrings = 0;

			for (auto& amx : amx::amx_list)
			{
				int callback = 0;
				if (amx_FindPublic(amx.second->get(), name.c_str(), &callback))
				{
					continue;
				}

				if (callback < -10000)
				{
					continue;
				}

				for (int i = format.length() - 1; i >= 0; i--)
				{
					switch (format[i])
					{
					case 'd':
					case 'i':
					{
						amx_Push(amx.second->get(), *reinterpret_cast<cell*>(params[i]));
					}
					break;
					case 'f':
					{
						amx_Push(amx.second->get(), *reinterpret_cast<cell*>(params[i]));
					}
					break;
					case 's':
					{
						char* string = static_cast<char*>(params[i]);
						if (string != 0 && strlen(string) > 0)
						{
							amx_PushString(amx.second->get(), &amx_addr[numberOfStrings], 0, string, 0, 0);
						}
						else
						{
							*string = 1;
							*(string + 1) = 0;
							amx_PushString(amx.second->get(), &amx_addr[numberOfStrings], 0, string, 0, 0);
						}

						numberOfStrings++;
					}
					break;
					case 'a':
					case 'v':
					{
						cell* amxArray = static_cast<cell*>(params[i]);
						amx_PushArray(amx.second->get(), &amx_addr[numberOfStrings], 0, amxArray, arraySizes[i]);
						numberOfStrings++;
					}
					break;
					default:
						break;
					}
				}

				js_calling_public = true;
				amx_Exec(amx.second->get(), reinterpret_cast<cell*>(&returnValue), callback);
				js_calling_public = false;

				while (numberOfStrings)
				{
					numberOfStrings--;
					amx_Release(amx.second->get(), amx_addr[numberOfStrings]);
				}
			}
		}
		info.GetReturnValue().Set(returnValue);
	}

	void callback::call_float(const v8::FunctionCallbackInfo<v8::Value>& info)
	{
		v8::Isolate* isolate = info.GetIsolate();
		v8::Locker locker(isolate);
		v8::Isolate::Scope isolateScope(isolate);

		auto _context = isolate->GetCurrentContext();
		v8::Context::Scope contextScope(_context);

		v8::TryCatch eh(isolate);
		int returnValue = 0;

		if (info.Length() > 0)
		{
			v8::String::Utf8Value str(isolate, info[0]);
			std::string name(*str);

			v8::String::Utf8Value str2(isolate, info[1]);
			std::string format(*str2);

			int k = 2;
			cell _params[32];
			std::vector<void*> params;
			int numberOfStrings = 0;
			int arraySizes[32];
			for (int i = 0; i < static_cast<int>(format.length()); i++)
			{
				switch (format[i])
				{
				case 'i':
				case 'd':
				{
					_params[i] = info[k]->Int32Value(_context).ToChecked();
					params.push_back(static_cast<void*>(&_params[i]));
					k++;
				}
				break;
				case 'f':
				{
					float val = 0.0;
					if (!info[k]->IsUndefined()) val = static_cast<float>(info[k]->NumberValue(_context).ToChecked());
					_params[i] = amx_ftoc(val);
					params.push_back(static_cast<void*>(&_params[i]));
					k++;
				}
				break;
				case 's':
				{
					v8::String::Utf8Value _str(isolate, info[k]);
					const char* str(*_str);
					size_t slen = strlen(str);
					char* mystr = new char[slen + 1];
					for (size_t x = 0; x < slen; x++)
					{
						mystr[x] = str[x];
					}
					mystr[slen] = '\0';
					params.push_back(static_cast<void*>(mystr));
					numberOfStrings++;
					k++;
				}
				break;
				case 'a':
				{
					if (!info[k]->IsArray())
					{
						info.GetReturnValue().Set(false);
						L_ERROR << "callPublic: '" << name << "', parameter " << k << "must be an array";
						return;
					}

					v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(info[k]);
					size_t size = a->Length();

					cell* value = new cell[size];

					for (size_t b = 0; b < size; b++)
					{
						value[b] = a->Get(_context, b).ToLocalChecked()->Int32Value(_context).ToChecked();
					}

					params.push_back(static_cast<void*>(value));
					arraySizes[i] = size;
					numberOfStrings++;
					k++;
				}
				break;
				case 'v':
				{
					if (!info[k]->IsArray())
					{
						info.GetReturnValue().Set(false);
						L_ERROR << "callPublic: '" << name << "', parameter " << k << "must be an array";
						return;
					}

					v8::Local<v8::Array> a = v8::Local<v8::Array>::Cast(info[k]);
					size_t size = a->Length();

					cell* value = new cell[size];

					for (size_t b = 0; b < size; b++)
					{
						float val = static_cast<float>(a->Get(_context, b).ToLocalChecked()->NumberValue(_context).ToChecked());
						value[b] = amx_ftoc(val);
					}

					params.push_back(static_cast<void*>(value));
					arraySizes[i] = size;
					numberOfStrings++;
					k++;
				}
				break;
				default:
					break;
				}
			}

			std::vector<cell> amx_addr;
			for (int i = 0; i < numberOfStrings; i++)
			{
				amx_addr.push_back(0);
			}
			numberOfStrings = 0;

			for (auto& amx : amx::amx_list)
			{
				int callback = 0;
				if (amx_FindPublic(amx.second->get(), name.c_str(), &callback))
				{
					continue;
				}

				if (callback < -10000)
				{
					continue;
				}

				for (int i = format.length() - 1; i >= 0; i--)
				{
					switch (format[i])
					{
					case 'd':
					case 'i':
					{
						amx_Push(amx.second->get(), *reinterpret_cast<cell*>(params[i]));
					}
					break;
					case 'f':
					{
						amx_Push(amx.second->get(), *reinterpret_cast<cell*>(params[i]));
					}
					break;
					case 's':
					{
						char* string = static_cast<char*>(params[i]);
						if (string != 0 && strlen(string) > 0)
						{
							amx_PushString(amx.second->get(), &amx_addr[numberOfStrings], 0, string, 0, 0);
						}
						else
						{
							*string = 1;
							*(string + 1) = 0;
							amx_PushString(amx.second->get(), &amx_addr[numberOfStrings], 0, string, 0, 0);
						}

						numberOfStrings++;
					}
					break;
					case 'a':
					case 'v':
					{
						cell* amxArray = static_cast<cell*>(params[i]);
						amx_PushArray(amx.second->get(), &amx_addr[numberOfStrings], 0, amxArray, arraySizes[i]);
						numberOfStrings++;
					}
					break;
					default:
						break;
					}
				}

				js_calling_public = true;
				amx_Exec(amx.second->get(), reinterpret_cast<cell*>(&returnValue), callback);
				js_calling_public = false;

				while (numberOfStrings)
				{
					numberOfStrings--;
					amx_Release(amx.second->get(), amx_addr[numberOfStrings]);
				}
			}
		}

		info.GetReturnValue().Set(v8::Number::New(isolate, amx_ctof(returnValue)));
	}
}