/***********************************************************************************************
 ***						W I N A P P  ---  D I S T E N G I N E                            ***
 ***********************************************************************************************
 *                                                                                             *
 *                                   Project Name : WinApp									   *
 *                                                                                             *
 *                                   File Name : Event.h                                       *
 *                                                                                             *
 *                                    Programmer : Kirk                                        *
 *                                                                                             *
 *                                     Date : 2022/8/3                                         *
 *                                                                                             *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#pragma once
#include <functional>
#include <string>
#include <unordered_map>

namespace EventSystem
{
	enum class EventType
	{
		null = 0,
		OnResize,
		OnMouseDown,
		OnMouseUp,
		OnMouseMove,
		OnKeyboardInput,
		Tick,
		Draw,
	};

	class Event
	{

	public:
		friend class EventDispatcher;

		virtual ~Event() {};

		virtual EventType GetEventType() = 0;
		virtual std::string GetDetail() = 0;

		bool GetHandleStatus() const { return bHandle; }

	private:
		bool bHandle;

	};

	class EventDispatcher
	{
		template<typename T>
		using EvenDispatchFunc = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) : m_Event(event) {}
		~EventDispatcher() {}

		template<typename T>
		void Dispatch(EvenDispatchFunc<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticEventType())
			{
				m_Event.bHandle = func(*(T*)&m_Event);
			}
		}
	private:
		Event& m_Event;
	};
}