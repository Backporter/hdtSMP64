namespace Events
{
    class FreezeEventHandler : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
	{
	public:
		static FreezeEventHandler*	GetSingleton();
		static void					Register();
		static void					Unregister();
	public:
		RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_eventSource) override;
	private:
	};

	//
	void Register();
	void Unregister();
}
