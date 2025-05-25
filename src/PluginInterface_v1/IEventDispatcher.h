#pragma once

#include "IEventListener.h"

namespace hdt::PluginInterface_v1
{
	template <class Event = void>
	class IEventDispatcher
	{
	public:

		virtual ~IEventDispatcher()
		{
		}

		virtual void addListener(hdt::PluginInterface_v1::IEventListener<Event>*) = 0;
		virtual void removeListener(hdt::PluginInterface_v1::IEventListener<Event>*) = 0;
		virtual void dispatch(const Event&) = 0;
	};

	template <>
	class IEventDispatcher<void>
	{
	public:

		virtual ~IEventDispatcher()
		{
		}

		virtual void addListener(hdt::PluginInterface_v1::IEventListener<void>*) = 0;
		virtual void removeListener(hdt::PluginInterface_v1::IEventListener<void>*) = 0;
		virtual void dispatch() = 0;
	};
}
