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
        ClassData()
            : m_dispatcher(buildEventHandlersMap(EventQueueId::eDefault))
            , m_queue(m_dispatcher)
        {
        }

        void stop();
        void sendEvent(const EventTypeEnum, BytePtr_t&&);
        void registerEventHandler(const EventHandlerId, IEventHandler* const);
        void unregisterEventHandler(const EventHandlerId);
        const BytePtr_t& getLastEventData(const EventTypeEnum) const;

    private:
        void start();

        EventDispatcher m_dispatcher;
        EventQueue m_queue;
        RegistrationTracker m_registrationTracker{};
        std::atomic<bool> m_stopCalled{false};
    };

    void EventSystem::ClassData::start()
    {
        m_queue.start();
    }

    void EventSystem::ClassData::stop()
    {
        m_stopCalled.store(true, std::memory_order_relaxed);
        m_queue.stop();
    }

    void EventSystem::ClassData::sendEvent(const EventTypeEnum eventId, BytePtr_t&& data)
    {
        m_queue.addEvent(eventId, std::move(data));
    }

    void EventSystem::ClassData::registerEventHandler(const EventHandlerId handlerId
        , IEventHandler* const handlerPtr)
    {
        if (handlerPtr == nullptr || m_registrationTracker.allRegistered())
        {
            return;
        }

        if (!m_registrationTracker.registerHandler(handlerId))
        {
            return;
        }

        m_dispatcher.setEventHandlerPtr(handlerId, handlerPtr);

        if (m_registrationTracker.allRegistered())
        {
            start();
            sendEvent(EventTypeEnum::eEventSystemReady, {});
        }
    }

    void EventSystem::ClassData::unregisterEventHandler(const EventHandlerId handlerId)
    {
        if (!m_stopCalled.load(std::memory_order_relaxed))
        {
            std::cout << "Cannot unregister '" << toString(handlerId) 
                << "', event system is still active! Stop it first.\n";
            return;
        }

        m_dispatcher.setEventHandlerPtr(handlerId, nullptr);
        m_registrationTracker.unregisterHandler(handlerId);
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

    void EventSystem::registerEventHandler(const EventHandlerId handlerId
        , IEventHandler* const handlerPtr)
    {
        m_pimpl->registerEventHandler(handlerId, handlerPtr);
    }

    void EventSystem::unregisterEventHandler(const EventHandlerId handlerId)
    {
        m_pimpl->unregisterEventHandler(handlerId);
    }

    const BytePtr_t& EventSystem::getLastEventData(const EventTypeEnum eventId) const
    {
        return m_pimpl->getLastEventData(eventId);
    }

}