#include "ActorManager.h"
#include "Hooks.h"
#include "HookEvents.h"

//
#include <xbyak/xbyak.h>

namespace Hooks
{
	void BSFaceGenNiNodeHooks::ProcessHeadPart(RE::BSFaceGenNiNode* const a_this, RE::BGSHeadPart* headPart, RE::NiNode* a_skeleton, bool a_unk)
	{
		//
		if (headPart) 
		{
			RE::NiAVObject* headNode = a_this->GetObjectByName(headPart->formEditorID);
			if (headNode) 
			{
				auto headGeo = headNode->AsTriShape();
				if (headGeo) 
				{
					SkinSingleGeometry__Hook(a_this, a_skeleton, headGeo, a_unk);
				}
			}

			//
			for (auto it : headPart->extraParts)
			{
				ProcessHeadPart(a_this, it, a_skeleton, a_unk);
			}
		}
	}

	void BSFaceGenNiNodeHooks::SkinAllGeometryCalls(RE::BSFaceGenNiNode* const a_this, RE::NiNode* a_skeleton, bool a_unk)
	{
		bool needRegularCall = true;
		if (hdt::ActorManager::instance()->skeletonNeedsParts(a_skeleton)) 
		{
			RE::TESForm* form = RE::TESForm::LookupByID(a_skeleton->GetUserData()->formID);
			RE::Actor* actor = skyrim_cast<RE::Actor*>(form);
			if (actor) 
			{
				RE::TESNPC* actorBase = skyrim_cast<RE::TESNPC*>(actor->data.objectReference);
				uint32_t numHeadParts = 0;
				RE::BGSHeadPart** Headparts = nullptr;

				if (actorBase->HasOverlays()) 
				{
					numHeadParts = actorBase->GetNumBaseOverlays();
					Headparts = actorBase->GetBaseOverlays();
				} 
				else 
				{
					numHeadParts = actorBase->numHeadParts;
					Headparts = actorBase->headParts;
				}

				if (Headparts) 
				{
					for (uint32_t i = 0; i < numHeadParts; i++) 
					{
						if (Headparts[i]) 
						{
							ProcessHeadPart(a_this, Headparts[i], a_skeleton, a_unk);
						}
					}
				}

				if (a_skeleton->GetUserData() && a_skeleton->GetUserData()->formID == 0x14) 
				{
					needRegularCall = false;
				}
			}
		}

		if (needRegularCall) 
		{
			SkinAllGeometry(a_this, a_skeleton, a_unk);
		}
	}

	void BSFaceGenNiNodeHooks::SkinSingleGeometry__Hook(RE::BSFaceGenNiNode* const a_this, RE::NiNode* a_skeleton, RE::BSTriShape* a_triShape, bool a_unk)
	{
		//
		const char* name = "";
		uint32_t formId = 0x0;

		//
		if (a_skeleton->GetUserData() && a_skeleton->GetUserData()->GetBaseObject()) 
		{
			auto bname = skyrim_cast<RE::TESFullName*>(a_skeleton->GetUserData()->GetBaseObject());
			if (bname) 
			{
				name = bname->GetFullName();
			}

			auto bnpc = skyrim_cast<RE::TESNPC*>(a_skeleton->GetUserData()->GetBaseObject());
			if (bnpc && bnpc->faceNPC) 
			{
				formId = bnpc->faceNPC->formID;
			}
		}

		//
		logger::debug("SkinSingleGeometry {} {} - {}, {}, (formid {:08x} base form {:08x} head template form {:08x})", a_skeleton->name.c_str(), a_skeleton->GetChildren().size(), a_triShape->name.c_str(), name, a_skeleton->GetUserData() ? a_skeleton->GetUserData()->formID : 0x0, a_skeleton->GetUserData() ? a_skeleton->GetUserData()->GetBaseObject()->formID : 0x0, formId);

		//
		hdt::SkinSingleHeadGeometryEvent e;
		e.headNode = a_this;
		e.skeleton = a_skeleton;
		e.geometry = a_triShape;

		//
		hdt::g_skinSingleHeadGeometryEventDispatcher.SendEvent(&e);
	}

	void BSFaceGenNiNodeHooks::SkinAllGeometry__Hook(RE::BSFaceGenNiNode* a_this, RE::NiNode* a_skeleton, bool a_unk)
	{
		//
		const char* name = "";
		uint32_t formId = 0x0;

		//
		if (a_skeleton->GetUserData() && a_skeleton->GetUserData()->data.objectReference) 
		{
			auto bname = skyrim_cast<RE::TESFullName*>(a_skeleton->GetUserData()->data.objectReference);
			if (bname) 
			{
				name = bname->GetFullName();
			}

			auto bnpc = skyrim_cast<RE::TESNPC*>(a_skeleton->GetUserData()->data.objectReference);
			if (bnpc && bnpc->faceNPC) 
			{
				formId = bnpc->faceNPC->formID;
			}
		}

		//
		logger::debug("SkinAllGeometry {} {}, {}, (formid {:08x} base form {:08x} head template form {:08x})", a_skeleton->name.c_str(), a_skeleton->GetChildren().size(), name, a_skeleton->GetUserData() ? a_skeleton->GetUserData()->formID : 0x0, a_skeleton->GetUserData() ? a_skeleton->GetUserData()->GetBaseObject()->formID : 0x0, formId);

		//
		hdt::SkinAllHeadGeometryEvent e;
		e.skeleton = a_skeleton;
		e.headNode = a_this;

		hdt::g_skinAllHeadGeometryEventDispatcher.SendEvent(&e);

		//
		if (REL::Module::IsAE())
		{
			SkinAllGeometryCalls(a_this, a_skeleton, a_unk);
		}
		else
		{
			SkinAllGeometry(a_this, a_skeleton, a_unk);
		}
		
		//
		e.hasSkinned = true;

		//
		hdt::g_skinAllHeadGeometryEventDispatcher.SendEvent(&e);
	}

	void BSFaceGenNiNodeHooks::SkinAllGeometry(RE::BSFaceGenNiNode* a_this, RE::NiNode* a_skeleton, bool a_unk)
	{
		if (a_skeleton)
		{
			const auto& children = a_this->GetChildren();
			if (children.size() > 0)
			{
				for (int i = 0; i < children.size(); i++)
				{
					auto child = children[i];
					if (child)
					{
						auto triShape = child->AsTriShape();
						if (triShape)
						{
							SkinSingleGeometry__Hook(a_this, a_skeleton, triShape, a_unk);
						}
					}
				}
			}
		}
	}

	void BSFaceGenNiNodeHooks::ApplyBoneLimitFix()
	{
		struct BoneLimitFix : Xbyak::CodeGenerator
		{
			BoneLimitFix(uintptr_t a_returnAddr) : CodeGenerator()
			{
				Xbyak::Label ret;

				//
				if (REL::Module::IsSE())
				{
					mov(esi, ptr[rax + 0x58]);
					cmp(esi, 9);
					jl(ret);
					mov(esi, 8);
				}
				else
				{
					mov(ebp, ptr[rax + 0x58]);
					cmp(ebp, 9);
					jl(ret);
					mov(ebp, 8);
				}

				//
				L(ret);
				jmp(ptr[rip]);
				dq(a_returnAddr);
			}
		};

		//
		BoneLimitFix code(GeometrySkinningBoneFix.address() + 5);

		//
		auto& localTrampoline = SKSE::GetTrampoline();
		SKSE::AllocTrampoline(14 + code.getSize());
		
		//
		localTrampoline.write_branch<5>(GeometrySkinningBoneFix.address(), localTrampoline.allocate(code));
	}
	
	void MainHooks::Update(RE::Main* a_this)
	{
		//
		_Update(a_this);

		// why doesn't this class have a GetRuntimeData() helper? the offsets are borked with VR support enabled.
		bool quitGame = REL::RelocateMember<bool>(a_this, 0x10, 0x8);

		//
		if (quitGame)
		{
			hdt::ShutdownEvent e;
			hdt::g_shutdownEventDispatcher.SendEvent(&e);
		} 
		else 
		{
			hdt::FrameEvent e;
			e.gamePaused = a_this->freezeTime;
			hdt::g_frameEventDispatcher.SendEvent(&e);
		}

	}

	void MainHooks::Unk_sub(void* a_this)
	{		
		_Unk_sub(a_this);

		//
		hdt::FrameSyncEvent framesyncEvent;
		hdt::g_frameSyncEventDispatcher.SendEvent(&framesyncEvent);
	}

	bool ActorEquipManagerHooks::UnequipObject
	(
		RE::ActorEquipManager* a_this, 
		RE::Actor* a_actor, 
		RE::TESBoundObject* a_object, 
		RE::ExtraDataList* a_extraData, 
		std::uint32_t a_count, 
		const RE::BGSEquipSlot* a_slot, 
		bool a_queueEquip, 
		bool a_forceEquip, 
		bool a_playSounds, 
		bool a_applyNow, 
		const RE::BGSEquipSlot* a_slotToReplace
	)
	{
		hdt::ArmorDetachEvent event;
		event.actor = a_actor;
		event.hasDetached = false;

		//
		hdt::g_armorDetachEventDispatcher.SendEvent(&event);

		//
		auto ret = _UnequipObject(a_this, a_actor, a_object, a_extraData, a_count, a_slot, a_queueEquip, a_forceEquip, a_playSounds, a_applyNow, a_slotToReplace);

		//
		event.hasDetached = true;

		//
		hdt::g_armorDetachEventDispatcher.SendEvent(&event);

		//
		return ret;
	}

	RE::NiNode* BipedAnimHooks::unk001CB0E0(RE::BipedAnim* a_this, RE::NiNode* armor, RE::BSFadeNode* skeleton, void* a_unk3, uint32_t a_unk4, uint32_t a_unk5, void* a_unk6)
	{
		hdt::ArmorAttachEvent event;
		event.armorModel = armor;
		event.skeleton = skeleton;
		event.attachedNode = nullptr;
		event.hasAttached = false;

		//
		hdt::g_armorAttachEventDispatcher.SendEvent(&event);

		//
		RE::NiNode* ret = _unk001CB0E0(a_this, armor, skeleton, a_unk3, a_unk4, a_unk5, a_unk6);

		//
		if (ret) 
		{
			event.attachedNode = ret;
			event.hasAttached = true;

			//
			hdt::g_armorAttachEventDispatcher.SendEvent(&event);
		}

		//
		return ret;
	}

	void Install()
	{
		logger::trace("Hooking...");

		//
		BSFaceGenNiNodeHooks::Hook();
		MainHooks::Hook();
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		ActorEquipManagerHooks::Hook();
		BipedAnimHooks::Hook();
		DetourTransactionCommit();

		//
		logger::trace("...success");
	}
}
