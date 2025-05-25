#pragma once

namespace hdt::PluginInterface_v1
{
	template <class Event = void>
	class IEventListener
	{
	public:
		virtual void onEvent(const Event&) = 0;
	};

	template <>
	class IEventListener<void>
	{
	public:
		virtual void onEvent() = 0;
	};
}
