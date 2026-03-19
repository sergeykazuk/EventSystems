#include "system/EventSystem.hpp"
#include "core/EventQueue.hpp"
#include "core/EventDispatcher.hpp"
#include "core/EventHandlersMapBuilder.hpp"
#include "core/EventQueueId.hpp"
#include "core/RegistrationTracker.hpp"
#include <atomic>
#include <iostream>

namespace event_system 
{

    struct EventSystem::ClassData
    {
        enum class State : uint8_t
        {
            eWaitingForHandlers = 0,
            eRunning,
            eStopped
        };

        ClassData()
            : m_dispatcher(buildEventHandlersMap(EventQueueId::eDefault))
            , m_queue(m_dispatcher)
        {
        }

        void stop();
        void sendEvent(const EventTypeEnum, BytePtr_t&&);
        EventSystemOperationResult 
            registerEventHandler(const EventHandlerId, IEventHandler* const);
        EventSystemOperationResult 
            unregisterEventHandler(const EventHandlerId);
        const BytePtr_t& getLastEventData(const EventTypeEnum) const;

    private:
        void start();

        EventDispatcher m_dispatcher;
        EventQueue m_queue;
        RegistrationTracker m_registrationTracker{};
        std::atomic<State> m_state{State::eWaitingForHandlers};
    };

    void EventSystem::ClassData::start()
    {
        auto expectedState{State::eWaitingForHandlers};
        if (m_state.compare_exchange_strong(expectedState, State::eRunning))
        {
            m_queue.start();
        }
    }

    void EventSystem::ClassData::stop()
    {
        const State previousState = m_state.exchange(State::eStopped);
        if (previousState == State::eStopped)
        {
            return;
        }

        m_queue.stop();
    }

    void EventSystem::ClassData::sendEvent(const EventTypeEnum eventId, BytePtr_t&& data)
    {
        m_queue.addEvent(eventId, std::move(data));
    }

    EventSystemOperationResult EventSystem::ClassData::registerEventHandler(const EventHandlerId handlerId
        , IEventHandler* const handlerPtr)
    {
        if (handlerPtr == nullptr)
        {
            return EventSystemOperationResult::eNullHandler;
        }

        if (m_state.load(std::memory_order_relaxed) != State::eWaitingForHandlers)
        {
            return EventSystemOperationResult::eInvalidState;
        }

        if (handlerId == EventHandlerId::eHandlersCount)
        {
            return EventSystemOperationResult::eOutOfRangeHandlerId;
        }

        if (!m_registrationTracker.registerHandler(handlerId))
        {
            return EventSystemOperationResult::eAlreadyRegistered;
        }

        m_dispatcher.setEventHandlerPtr(handlerId, handlerPtr);

        if (m_registrationTracker.allRegistered())
        {
            start();
            sendEvent(EventTypeEnum::eEventSystemReady, {});
        }

        return EventSystemOperationResult::eSuccess;
    }

    EventSystemOperationResult EventSystem::ClassData::unregisterEventHandler(const EventHandlerId handlerId)
    {
        if (m_state.load(std::memory_order_relaxed) != State::eStopped)
        {
            std::cout << "Cannot unregister '" << toString(handlerId) 
                << "', event system is still active! Stop it first.\n";
            return EventSystemOperationResult::eInvalidState;
        }

        if (handlerId == EventHandlerId::eHandlersCount)
        {
            return EventSystemOperationResult::eOutOfRangeHandlerId;
        }

        m_dispatcher.setEventHandlerPtr(handlerId, nullptr);
        m_registrationTracker.unregisterHandler(handlerId);
        return EventSystemOperationResult::eSuccess;
    }

    const BytePtr_t& EventSystem::ClassData::getLastEventData(const EventTypeEnum eventId) const
    {
        return m_queue.getLastEventData(eventId);
    }

    EventSystem::EventSystem()
        : m_pimpl(std::make_unique<ClassData>())
    {}


    EventSystem& EventSystem::getInstance()
    {
        static EventSystem sEv{};
        return sEv;
    }

    void EventSystem::sendEvent(const EventTypeEnum eventId, BytePtr_t&& data)
    {
        m_pimpl->sendEvent(eventId, std::move(data));
    }

    void EventSystem::init()
    {
    }

    void EventSystem::shutdown()
    {
        m_pimpl->stop();
    }

    EventSystemOperationResult EventSystem::registerEventHandler(const EventHandlerId handlerId
        , IEventHandler* const handlerPtr)
    {
        return m_pimpl->registerEventHandler(handlerId, handlerPtr);
    }

    EventSystemOperationResult EventSystem::unregisterEventHandler(const EventHandlerId handlerId)
    {
        return m_pimpl->unregisterEventHandler(handlerId);
    }

    const BytePtr_t& EventSystem::getLastEventData(const EventTypeEnum eventId) const
    {
        return m_pimpl->getLastEventData(eventId);
    }

}