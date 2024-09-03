#include "HookEvents.h"

namespace hdt
{
	RE::BSTEventSource<FrameEvent> g_frameEventDispatcher;
	RE::BSTEventSource<FrameSyncEvent> g_frameSyncEventDispatcher;
	RE::BSTEventSource<ShutdownEvent> g_shutdownEventDispatcher;
	RE::BSTEventSource<ArmorAttachEvent> g_armorAttachEventDispatcher;
	RE::BSTEventSource<ArmorDetachEvent> g_armorDetachEventDispatcher;
	RE::BSTEventSource<SkinAllHeadGeometryEvent> g_skinAllHeadGeometryEventDispatcher;
	RE::BSTEventSource<SkinSingleHeadGeometryEvent> g_skinSingleHeadGeometryEventDispatcher;
}
