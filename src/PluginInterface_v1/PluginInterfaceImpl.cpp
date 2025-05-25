#include "PluginInterfaceImpl.h"
#include "../PluginInterfaceImpl.h"

hdt::PluginInterface_v1::PluginInterfaceImpl hdt::g_pluginInterface_v1;

void hdt::PluginInterface_v1::PluginInterfaceImpl::addListener(hdt::PluginInterface_v1::IPreStepListener* l)
{
	if (l)
	{
		m_preStepDispatcher.addListener(l);
	}
}

void hdt::PluginInterface_v1::PluginInterfaceImpl::removeListener(hdt::PluginInterface_v1::IPreStepListener* l)
{
	if (l)
	{
		m_preStepDispatcher.removeListener(l);
	}
}

void hdt::PluginInterface_v1::PluginInterfaceImpl::addListener(hdt::PluginInterface_v1::IPostStepListener* l)
{
	if (l)
	{
		m_postStepDispatcher.addListener(l);
	}
}

void hdt::PluginInterface_v1::PluginInterfaceImpl::removeListener(hdt::PluginInterface_v1::IPostStepListener* l)
{
	if (l)
	{
		m_postStepDispatcher.removeListener(l);
	}
}

void hdt::PluginInterface_v1::PluginInterfaceImpl::onPostPostLoad()
{
	// Send ourselves to any plugin that registered during the PostLoad event
	if (m_skseMessagingInterface)
	{
		m_skseMessagingInterface->Dispatch(hdt::PluginInterface_v1::PluginInterface::MSG_STARTUP, static_cast<hdt::PluginInterface_v1::PluginInterface*>(this), 0, nullptr);
	}
}

void hdt::PluginInterface_v1::PluginInterfaceImpl::init(const SKSE::LoadInterface* skse)
{
	//We need to have our SKSE plugin handle and the messaging interface in order to reach our plugins later
	if (skse)
	{
		m_sksePluginHandle = skse->GetPluginHandle();
		m_skseMessagingInterface = reinterpret_cast<SKSE::MessagingInterface*>(skse->QueryInterface(SKSE::LoadInterface::kMessaging));
	}
	if (!m_skseMessagingInterface)
	{
		logger::warn("Failed to get a messaging interface. Plugins will not work.");
	}
}
