#include "PluginInterfaceImpl.h"

hdt::PluginInterfaceImpl hdt::g_pluginInterface;

namespace
{

	class LegacyPreStepEventSink : public RE::BSTEventSink<hdt::PreStepEvent>
	{
	public:
		LegacyPreStepEventSink(hdt::IPreStepListener* a_legacyListner) :
			legacyListner(a_legacyListner) {};

		hdt::IPreStepListener* legacyListner;

		virtual RE::BSEventNotifyControl ProcessEvent(const hdt::PreStepEvent* a_event, RE::BSTEventSource<hdt::PreStepEvent>* a_eventSource)
		{
			legacyListner->onEvent(*a_event);
			return RE::BSEventNotifyControl::kContinue;
		}
	};

	class LegacyPostStepEventSink : public RE::BSTEventSink<hdt::PostStepEvent>
	{
	public:
		LegacyPostStepEventSink(hdt::IPostStepListener* a_legacyListner) :
			legacyListner(a_legacyListner) {};

		hdt::IPostStepListener* legacyListner;

		virtual RE::BSEventNotifyControl ProcessEvent(const hdt::PostStepEvent* a_event, RE::BSTEventSource<hdt::PostStepEvent>* a_eventSource)
		{
			legacyListner->onEvent(*a_event);
			return RE::BSEventNotifyControl::kContinue;
		}
	};
}

void hdt::PluginInterfaceImpl::addListener(IPreStepListener* l)
{
	if (l)
	{
		m_preStepDispatcher.AddEventSink(new LegacyPreStepEventSink(l));
	}
}

void hdt::PluginInterfaceImpl::removeListener(IPreStepListener* l)
{
	if (l) {
		// TODO: Complicated... (and not tested)
		RE::BSSpinLockGuard locker(m_preStepDispatcher.lock);

		if (m_preStepDispatcher.notifying) {
			LegacyPreStepEventSink* found = nullptr;
			for (const auto& event : m_preStepDispatcher.pendingUnregisters) {
				if (LegacyPreStepEventSink* legacyEvent = dynamic_cast<LegacyPreStepEventSink*>(event)) {
					if (legacyEvent->legacyListner == l) {
						found = legacyEvent;
						break;
					}
				}
			}
			if (!found) {
				m_preStepDispatcher.pendingUnregisters.push_back(found);
			}
		} else {
			RE::BSTEventSink<hdt::PreStepEvent>** found_it = nullptr;
			for (RE::BSTEventSink<hdt::PreStepEvent>** it = m_preStepDispatcher.sinks.begin(); it != m_preStepDispatcher.sinks.end(); ++it) {
				if (LegacyPreStepEventSink* legacyEvent = dynamic_cast<LegacyPreStepEventSink*>(*it)) {
					if (legacyEvent->legacyListner == l) {
						found_it = it;
						break;
					}
				}
			}
			if (found_it) {
				m_preStepDispatcher.sinks.erase(found_it);
			}
		}

		RE::BSTEventSink<hdt::PreStepEvent>** found_it = nullptr;
		for (RE::BSTEventSink<hdt::PreStepEvent>** it = m_preStepDispatcher.pendingRegisters.begin(); it != m_preStepDispatcher.pendingRegisters.end(); ++it) {
			if (LegacyPreStepEventSink* legacyEvent = dynamic_cast<LegacyPreStepEventSink*>(*it)) {
				if (legacyEvent->legacyListner == l) {
					found_it = it;
					break;
				}
			}
		}
		if (found_it) {
			m_preStepDispatcher.pendingRegisters.erase(found_it);
		}
	}
}

void hdt::PluginInterfaceImpl::addListener(IPostStepListener* l)
{
	if (l)
	{
		m_postStepDispatcher.AddEventSink(new LegacyPostStepEventSink(l));
	}
}

void hdt::PluginInterfaceImpl::removeListener(IPostStepListener* l)
{
	if (l)
	{
		// TODO: 
	}
}

void hdt::PluginInterfaceImpl::onPostPostLoad()
{
	// Send ourselves to any plugin that registered during the PostLoad event
	if (m_skseMessagingInterface)
	{
		m_skseMessagingInterface->Dispatch(PluginInterface::MSG_STARTUP, static_cast<PluginInterface*>(this), 0, nullptr);
	}
}

void hdt::PluginInterfaceImpl::init(const SKSE::LoadInterface* skse)
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
