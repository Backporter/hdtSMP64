#pragma once

#include <detours/detours.h>

namespace Hooks
{
	class BSFaceGenNiNodeHooks
	{
	public:
		/* hooks/patches */

		static void Hook()
		{
			//
			logger::debug("Applying BSFaceGenNiNodeHooks hooks!");

			//
			auto& trampoline = SKSE::GetTrampoline();

			//
			BSFaceGenNiNode__vtbl.write_vfunc(0x3E, SkinAllGeometry__Hook);

			//
			SKSE::AllocTrampoline(14);
			_SkinSingleGeometry = trampoline.write_call<5>(SkinSingleGeometryCode1.address(), SkinSingleGeometry__Hook);

			//
			REL::safe_write(_SkinSingleGeometry.address() + REL::Offset(0x96).offset(), static_cast<uint8_t>(0x7));

			//
			ApplyBoneLimitFix();

			//
			logger::debug("...success");
		}

		static void ApplyBoneLimitFix();
	public:
		/* Defined functions */
		static void ProcessHeadPart(RE::BSFaceGenNiNode*, RE::BGSHeadPart*, RE::NiNode*, bool);
		static void SkinAllGeometryCalls(RE::BSFaceGenNiNode*, RE::NiNode*, bool);

		static void SkinSingleGeometry__Hook(RE::BSFaceGenNiNode*, RE::NiNode*, RE::BSTriShape*, bool);
		static void SkinAllGeometry__Hook(RE::BSFaceGenNiNode*, RE::NiNode*, bool a_unk);

		//
		static void SkinAllGeometry(RE::BSFaceGenNiNode*, RE::NiNode*, bool a_unk);
 	public:
		/* Relocations, offsets, functions, etc */
		static inline REL::Relocation<uintptr_t> BSFaceGenNiNode__vtbl { RE::VTABLE_BSFaceGenNiNode[0] };
		static inline REL::Relocation<uintptr_t> SkinSingleGeometryCode1 { REL::VariantID(26466, 27061, 0x03EBB30), REL::VariantOffset(0x108, 0x10F, 0x108) };	// 0x03dc1c0, 0x03F6770, 0x03EBB30 (SE/1.5.97.0, AE/1.6.640.0, VR/1.4.15.0)
		static inline REL::Relocation<uintptr_t> GeometrySkinningBoneFix { REL::VariantID(26406, 26987, 0x03E81B0), REL::Offset(0x094) };						// 0x03d8840, 0x03F2B20, 0x03E81B0 (SE/1.5.97.0, AE/1.6.640.0, VR/1.4.15.0)
	public:
		static inline REL::Relocation<decltype(&BSFaceGenNiNodeHooks::SkinSingleGeometry__Hook)> _SkinSingleGeometry;
	};

	class MainHooks
	{
	public:
		static void Hook()
		{
			logger::debug("Applying MainHooks hooks!");

			//
			auto& trampoline = SKSE::GetTrampoline();

			//
			SKSE::AllocTrampoline(14);
			_Update = trampoline.write_call<5>(UpdateHook1.address(), Update);
			
			SKSE::AllocTrampoline(14);
			_Unk_sub = trampoline.write_call<5>(UpdateHook2.address(), Unk_sub);

			logger::debug("...success");
		}

		static void Update(RE::Main*);
		static void Unk_sub(void*);  // RE::BSBethesdaPlatform*
	public:
		//
		static inline REL::Relocation<uintptr_t> UpdateHook1 { REL::VariantID(35551, 36550, 0x05B6D70), REL::Offset(0x11F) };						// 0x05AF3D0, 0x05E7EE0, 0x05B6D70 (SE/1.5.97.0, AE/1.6.640.0, VR/1.4.15.0)
		static inline REL::Relocation<uintptr_t> UpdateHook2 { REL::VariantID(35565, 36564, 0x05BAB10), REL::VariantOffset(0x56D, 0x9DC, 0x611) };  // 0x05B2FF0, 0x05EC240, 0x05BAB10 (SE/1.5.97.0, AE/1.6.640.0, VR/1.4.15.0)

		//
		static inline REL::Relocation<decltype(&Unk_sub)> _Unk_sub;
		static inline REL::Relocation<decltype(&Update)> _Update;
	};

	class ActorEquipManagerHooks
	{
	public:
		static void Hook()
		{
			logger::debug("Applying ActorEquipManagerHooks hooks!");

			//
			DetourAttach(reinterpret_cast<PVOID*>(&_UnequipObject), (PVOID)UnequipObject);

			logger::debug("...success");
		}

		static bool UnequipObject(RE::ActorEquipManager* a_this, RE::Actor* a_actor, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraData = nullptr, std::uint32_t a_count = 1, const RE::BGSEquipSlot* a_slot = nullptr, bool a_queueEquip = true, bool a_forceEquip = false, bool a_playSounds = true, bool a_applyNow = false, const RE::BGSEquipSlot* a_slotToReplace = nullptr);
	public:
		static inline REL::Relocation<decltype(&UnequipObject)> _UnequipObject { REL::VariantID(37945, 38901, 0x06411A0) }; // 0x0638190, 0x0670210, 0x06411A0 (SE/1.5.97.0, AE/1.6.640.0, VR/1.4.15.0)
	};

	class BipedAnimHooks
	{
	public:
		static void Hook()
		{
			logger::debug("Applying BipedAnimHooks hooks!");

			DetourAttach(reinterpret_cast<PVOID*>(&_unk001CB0E0), (PVOID)unk001CB0E0);

			logger::debug("...success");
		}

		static RE::NiNode* unk001CB0E0(RE::BipedAnim*, RE::NiNode*, RE::BSFadeNode*, void*, uint32_t, uint32_t, void*);
	public:
		static inline REL::Relocation<decltype(&unk001CB0E0)> _unk001CB0E0 { REL::VariantID(15535, 15712, 0x01DB9E0) }; // 0x01CAFB0, 0x01D83B0, 0x01DB9E0 (SE/1.5.97.0, AE/1.6.640.0, VR/1.4.15.0)
	};

    void Install();
}
