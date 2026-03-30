#include "core/EventSystem.hpp"
#include "core/EventQueue.hpp"
#include "core/EventDispatcher.hpp"
#include "core/EventHandlersMapBuilder.hpp"
#include "core/EventQueueId.hpp"
#include "core/RegistrationTracker.hpp"
#include "core/EventSystemTimer.hpp"
#include "core/EventPayloadEraser.hpp"
#include "types/TimedData.hpp"
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
            , m_timer([this](std::vector<EventTypeEnum> t) {
                onTimerTimeout(std::move(t));
            })
        {
        }

        void stop();
        void sendEvent(const EventTypeEnum, BytePtr_t&&);
        EventSystemOperationResult 
            registerEventHandler(const EventHandlerId, IEventHandler* const);
        EventSystemOperationResult 
            unregisterEventHandler(const EventHandlerId);
        bool getLastEventData(const EventTypeEnum,
            const std::function<void(std::byte const * const)>& visitor) const;
        void sendTimedEvent(const EventTypeEnum, std::chrono::milliseconds);
        void stopTimedEvent(const EventTypeEnum);

    private:
        void start();
        void onTimerTimeout(std::vector<EventTypeEnum>);

        EventDispatcher m_dispatcher;
        EventQueue m_queue;
        EventSystemTimer m_timer;
        RegistrationTracker m_registrationTracker{};
        std::atomic<State> m_state{State::eWaitingForHandlers};
    };

    void EventSystem::ClassData::start()
    {
        m_timer.init();

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

        // Shutdown discards queued events to avoid dispatching while handlers
        // may already be in destruction/unregistration paths.
        m_queue.stop();
        m_timer.shutdown();
    }

    void EventSystem::ClassData::sendEvent(const EventTypeEnum eventId, BytePtr_t&& data)
    {
        m_queue.addEvent(eventId, std::move(data));
    }

    void EventSystem::ClassData::sendTimedEvent(const EventTypeEnum event
        , std::chrono::milliseconds timeout)
    {
        if (m_state.load(std::memory_order_relaxed) != State::eRunning)
        {
            std::cout << "Cannot start timed event while EventSystem is not running\n";
            return;
        }

        m_timer.startTimer(event, timeout);
    }

    void EventSystem::ClassData::stopTimedEvent(const EventTypeEnum event)
    {
        if (m_state.load(std::memory_order_relaxed) != State::eRunning)
        {
            std::cout << "Cannot stop timed event while EventSystem is not running\n";
            return;
        }

        m_timer.stopTimer(event);
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

    bool EventSystem::ClassData::getLastEventData(const EventTypeEnum eventId,
        const std::function<void(std::byte const * const)>& visitor) const
    {
        return m_queue.getLastEventData(eventId, visitor);
    }

    void EventSystem::ClassData::onTimerTimeout(std::vector<EventTypeEnum> vec)
    {
        for (const auto event : vec)
        {
            // at the moment send always as 1, since there is no support for
            // repetitive timed events
            sendEvent(event, erasePayload(TimedData{1}));
        }
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

    bool EventSystem::getLastEventData(const EventTypeEnum eventId,
        const std::function<void(std::byte const * const)>& visitor) const
    {
        return m_pimpl->getLastEventData(eventId, visitor);
    }

    void EventSystem::sendTimedEvent(const EventTypeEnum event
        , std::chrono::milliseconds timeout)
    {
        m_pimpl->sendTimedEvent(event, timeout);
    }

    void EventSystem::stopTimedEvent(const EventTypeEnum event)
    {
        m_pimpl->stopTimedEvent(event);
    }

}