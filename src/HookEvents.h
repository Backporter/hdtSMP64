#pragma once

namespace hdt
{
	struct SkinAllHeadGeometryEvent
	{
		RE::NiNode* skeleton = nullptr;
		RE::BSFaceGenNiNode* headNode = nullptr;
		bool hasSkinned = false;
	};

	struct SkinSingleHeadGeometryEvent
	{
		RE::NiNode* skeleton = nullptr;
		RE::BSFaceGenNiNode* headNode = nullptr;
		RE::BSGeometry* geometry = nullptr;
	};

	struct ArmorAttachEvent
	{
		RE::NiNode* armorModel = nullptr;
		RE::NiNode* skeleton = nullptr;
		RE::NiAVObject* attachedNode = nullptr;
		bool hasAttached = false;
	};

	struct ArmorDetachEvent
	{
		RE::Actor* actor = nullptr;
		bool hasDetached = false;
	};

	struct FrameEvent
	{
		bool gamePaused;
	};

	struct FrameSyncEvent
	{
	};

	struct ShutdownEvent
	{
	};

	extern RE::BSTEventSource<FrameEvent> g_frameEventDispatcher;
	extern RE::BSTEventSource<FrameSyncEvent> g_frameSyncEventDispatcher;
	extern RE::BSTEventSource<ShutdownEvent> g_shutdownEventDispatcher;
	extern RE::BSTEventSource<ArmorAttachEvent> g_armorAttachEventDispatcher;
	extern RE::BSTEventSource<ArmorDetachEvent> g_armorDetachEventDispatcher;
	extern RE::BSTEventSource<SkinAllHeadGeometryEvent> g_skinAllHeadGeometryEventDispatcher;
	extern RE::BSTEventSource<SkinSingleHeadGeometryEvent> g_skinSingleHeadGeometryEventDispatcher;
}
