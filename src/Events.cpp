#include "Events.h"
#include "hdtSkyrimPhysicsWorld.h"
#include "ActorManager.h"

namespace Events
{
    FreezeEventHandler* FreezeEventHandler::GetSingleton()
    {
        static FreezeEventHandler singleton;
        return std::addressof(singleton);
    } 

    void FreezeEventHandler::Register()
    {
        RE::UI::GetSingleton()->AddEventSink(GetSingleton());
    }

    void FreezeEventHandler::Unregister()
    {
        RE::UI::GetSingleton()->RemoveEventSink(GetSingleton());
    }

    RE::BSEventNotifyControl FreezeEventHandler::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, [[maybe_unused]] RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource)
    {
		if (a_event && a_event->opening && (a_event->menuName == "Loading Menu" || a_event->menuName == "RaceSex Menu"))
		{
			logger::debug("{} detected, scheduling physics reset on world un-suspend.", a_event->menuName);
			hdt::SkyrimPhysicsWorld::get()->suspend(true);
		}

		if (a_event && !a_event->opening && (a_event->menuName == "RaceSex Menu"))
		{
			logger::debug("Racemenu closed, reloading meshes.");
			hdt::ActorManager::instance()->ProcessEvent(a_event, a_eventSource);
		}

        return RE::BSEventNotifyControl::kContinue;
    }

    void Register()
    {
        FreezeEventHandler::Register();
    }

	void Unregister()
    {
        FreezeEventHandler::Unregister();
    }
}
