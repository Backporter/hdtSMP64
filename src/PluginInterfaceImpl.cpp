#include "PluginInterfaceImpl.h"

hdt::PluginInterfaceImpl hdt::g_pluginInterface;

void hdt::PluginInterfaceImpl::addListener(IPreStepListener* l)
{
	if (l)
	{
		m_preStepDispatcher.AddEventSink(l);
	}
}

void hdt::PluginInterfaceImpl::removeListener(IPreStepListener* l)
{
	if (l)
	{
		m_preStepDispatcher.RemoveEventSink(l);
	}
}

void hdt::PluginInterfaceImpl::addListener(IPostStepListener* l)
{
	if (l)
	{
		m_postStepDispatcher.AddEventSink(l);
	}
}

void hdt::PluginInterfaceImpl::removeListener(IPostStepListener* l)
{
	if (l)
	{
		m_postStepDispatcher.RemoveEventSink(l);
	}
}

void hdt::PluginInterfaceImpl::onPostPostLoad()
{
	// Send ourselves to any plugin that registered during the PostLoad event
	auto messagingInterface = SKSE::GetMessagingInterface();
	if (messagingInterface)
	{
		PluginInterface* data = static_cast<PluginInterface*>(this);
		messagingInterface->Dispatch(PluginInterface::MSG_STARTUP, (void*)data, 0, nullptr);
	}
}